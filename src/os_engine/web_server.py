#!/usr/bin/env python3
"""
ECO-CLOUD OS Engine Web Visualization Server
Real-time dashboard showing job scheduling, resource allocation, and Banker's Algorithm
"""

from flask import Flask, render_template, jsonify, request
from flask_cors import CORS
import json
import os
import subprocess
from pathlib import Path
import threading
import time

app = Flask(__name__)
CORS(app)

# Configuration
OS_ENGINE_DIR = os.path.dirname(os.path.abspath(__file__))
JSON_FILE = os.path.join(OS_ENGINE_DIR, 'sim_events.json')
LIVE_JSONL_FILE = os.path.join(OS_ENGINE_DIR, 'sim_events_live.jsonl')
EXECUTABLE = os.path.join(OS_ENGINE_DIR, 'os_core')

# Global state
simulation_in_progress = False
simulation_events = []

def load_simulation_data():
    """Load simulation events from JSON file"""
    global simulation_events
    try:
        if os.path.exists(JSON_FILE):
            with open(JSON_FILE, 'r') as f:
                data = json.load(f)
                simulation_events = data.get('events', [])
                return simulation_events
    except Exception as e:
        print(f"Error loading simulation data: {e}")
    return []

def run_simulation():
    """Run the C simulation and generate JSON logs"""
    global simulation_in_progress
    try:
        simulation_in_progress = True
        result = subprocess.run([EXECUTABLE], cwd=OS_ENGINE_DIR, 
                              capture_output=True, timeout=60, text=True)
        print("Simulation completed successfully")
        print("STDOUT:", result.stdout[-500:] if len(result.stdout) > 500 else result.stdout)
        if result.returncode != 0:
            print("STDERR:", result.stderr)
        load_simulation_data()
    except subprocess.TimeoutExpired:
        print("Simulation timed out")
    except Exception as e:
        print(f"Error running simulation: {e}")
    finally:
        simulation_in_progress = False

@app.route('/')
def index():
    """Serve the main dashboard"""
    return render_template('dashboard.html')

@app.route('/api/simulation/status')
def get_status():
    """Get current simulation status"""
    return jsonify({
        'in_progress': simulation_in_progress,
        'events_loaded': len(simulation_events),
        'json_file': os.path.exists(JSON_FILE),
        'live_file': os.path.exists(LIVE_JSONL_FILE)
    })

@app.route('/api/simulation/run', methods=['POST'])
def run_sim():
    """Start a new simulation"""
    if simulation_in_progress:
        return jsonify({'error': 'Simulation already running'}), 409
    
    # Run in background thread
    thread = threading.Thread(target=run_simulation, daemon=True)
    thread.start()
    
    return jsonify({'status': 'Simulation started'})

@app.route('/api/simulation/events')
def get_events():
    """Get all simulation events"""
    if not simulation_events:
        load_simulation_data()
    return jsonify({
        'events': simulation_events,
        'total': len(simulation_events)
    })

@app.route('/api/simulation/stats')
def get_stats():
    """Calculate simulation statistics"""
    if not simulation_events:
        load_simulation_data()
    
    stats = {
        'total_events': len(simulation_events),
        'jobs_created': 0,
        'successful_jobs': 0,
        'failed_jobs': 0,
        'resource_grants': 0,
        'resource_denials': 0,
        'total_duration_ms': 0,
        'state_transitions': {},
        'resource_summary': {
            'total_solar_allocated': 0,
            'total_grid_allocated': 0,
            'total_battery_allocated': 0,
            'final_solar_available': 0,
            'final_grid_available': 0,
            'final_battery_available': 0
        }
    }
    
    for event in simulation_events:
        event_type = event.get('type', '')
        
        if event_type == 'JOB_CREATED':
            stats['jobs_created'] += 1
        elif event_type == 'STATE_TRANSITION':
            to_state = event.get('to', '')
            stats['state_transitions'][to_state] = stats['state_transitions'].get(to_state, 0) + 1
            if to_state == 'TERMINATED':
                stats['successful_jobs'] += 1
        elif event_type == 'RESOURCE_GRANTED':
            stats['resource_grants'] += 1
            stats['resource_summary']['total_solar_allocated'] += event.get('asol', 0)
            stats['resource_summary']['total_grid_allocated'] += event.get('agrid', 0)
            stats['resource_summary']['total_battery_allocated'] += event.get('abat', 0)
        elif event_type == 'RESOURCE_DENIED':
            stats['resource_denials'] += 1
        elif event_type == 'RESOURCE_RELEASED':
            stats['resource_summary']['final_solar_available'] = event.get('sol', 0)
            stats['resource_summary']['final_grid_available'] = event.get('grid', 0)
            stats['resource_summary']['final_battery_available'] = event.get('bat', 0)
    
    if simulation_events:
        stats['total_duration_ms'] = simulation_events[-1].get('ts', 0)
    
    return jsonify(stats)

@app.route('/api/simulation/timeline')
def get_timeline():
    """Get timeline view of jobs"""
    if not simulation_events:
        load_simulation_data()
    
    jobs = {}
    
    for event in simulation_events:
        job_id = event.get('job', '')
        if not job_id:
            continue
        
        if job_id not in jobs:
            jobs[job_id] = {
                'id': job_id,
                'events': [],
                'states': [],
                'start_time': event.get('ts', 0),
                'end_time': event.get('ts', 0),
                'process_id': event.get('pid', -1),
                'resource_status': 'PENDING',
                'mode_history': []
            }
        
        job = jobs[job_id]
        job['events'].append(event)
        job['end_time'] = max(job['end_time'], event.get('ts', 0))
        
        if event.get('type') == 'STATE_TRANSITION':
            job['states'].append({
                'from': event.get('from', ''),
                'to': event.get('to', ''),
                'time': event.get('ts', 0),
                'core': event.get('core', 0)
            })
        elif event.get('type') == 'RESOURCE_GRANTED':
            job['resource_status'] = 'GRANTED'
        elif event.get('type') == 'RESOURCE_DENIED':
            job['resource_status'] = 'DENIED'
        elif event.get('type') == 'MODE_SWITCH':
            job['mode_history'].append({
                'mode': event.get('mode', ''),
                'time': event.get('ts', 0)
            })
    
    return jsonify({
        'jobs': list(jobs.values()),
        'total': len(jobs)
    })

@app.route('/api/simulation/resources')
def get_resources():
    """Get resource allocation timeline"""
    if not simulation_events:
        load_simulation_data()
    
    timeline = []
    
    for event in simulation_events:
        if event.get('type') in ['RESOURCE_GRANTED', 'RESOURCE_DENIED', 'RESOURCE_RELEASED']:
            timeline.append({
                'time': event.get('ts', 0),
                'type': event.get('type', ''),
                'job': event.get('job', ''),
                'solar_available': event.get('sol', 0),
                'grid_available': event.get('grid', 0),
                'battery_available': event.get('bat', 0),
                'solar_allocated': event.get('asol', 0),
                'grid_allocated': event.get('agrid', 0),
                'battery_allocated': event.get('abat', 0)
            })
    
    return jsonify({
        'timeline': timeline,
        'total': len(timeline)
    })

@app.route('/api/simulation/clear', methods=['POST'])
def clear_sim():
    """Clear simulation data"""
    global simulation_events
    simulation_events = []
    try:
        if os.path.exists(JSON_FILE):
            os.remove(JSON_FILE)
        if os.path.exists(LIVE_JSONL_FILE):
            os.remove(LIVE_JSONL_FILE)
    except Exception as e:
        print(f"Error clearing files: {e}")
    return jsonify({'status': 'Cleared'})

if __name__ == '__main__':
    # Ensure templates directory exists
    templates_dir = os.path.join(OS_ENGINE_DIR, 'templates')
    os.makedirs(templates_dir, exist_ok=True)
    
    # Load any existing data
    load_simulation_data()
    
    # Run Flask server
    port = int(os.environ.get('PORT', '5000'))
    print(f"Starting ECO-CLOUD OS Engine Dashboard on http://localhost:{port}")
    print(f"OS Engine dir: {OS_ENGINE_DIR}")
    print(f"Executable: {EXECUTABLE}")
    app.run(debug=True, host='0.0.0.0', port=port, use_reloader=False)
