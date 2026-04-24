# ECO-CLOUD OS Engine Web Visualization - Setup Guide

## 🎯 Overview

This guide walks you through setting up and running the **ECO-CLOUD OS Engine Web Dashboard** - a beautiful, real-time visualization of OS scheduling, resource allocation, and Banker's Algorithm.

## 📋 What You'll Get

A professional web dashboard showing:

```
Real-Time Visualizations:
├── 📋 Job Timeline (State Transitions)
├── ⚡ Resource Gauges (Solar/Grid/Battery)
├── 🖥️ CPU Core Monitor
├── 🏦 Banker's Algorithm Status
├── 👥 Process Queue Animation
└── 📊 Statistics Dashboard
```

## 🚀 Quick Start (30 seconds)

### Option 1: Automated Script (Recommended)

```bash
cd /workspaces/Eco-Cloud/src/os_engine
chmod +x run_dashboard.sh
./run_dashboard.sh
```

Then open **http://localhost:5000** in your browser.

### Option 2: Manual Setup

```bash
# Navigate to os_engine directory
cd /workspaces/Eco-Cloud/src/os_engine

# Compile the C simulation (if not already done)
gcc -pthread -o os_core os_core.c -Wall -Wextra

# Install Python dependencies
pip install flask flask-cors

# Start the web server
python3 web_server.py

# Open browser
open http://localhost:5000  # macOS
# or
xdg-open http://localhost:5000  # Linux
# or
start http://localhost:5000  # Windows
```

## 🎨 Dashboard Features Explained

### 📋 Job Timeline
Shows each job's journey through the OS:
- **NEW** → Admission control accepts job
- **READY** → Job queued and waiting
- **RUNNING** → Job executing on a CPU core
- **TERMINATED** → Job finished

**In the Dashboard**: Animated list showing state transitions with timestamps and resource status.

### ⚡ Resource Allocation Gauges
Three energy sources tracked in real-time:
- **☀️ Solar Power** (0-100W): Renewable energy
- **🔌 Grid Power** (0-200W): Primary power source
- **🔋 Battery Power** (0-50W): Backup/Reserve

**Visual**: Smooth animated bars filling up as resources are allocated.

### 🖥️ CPU Core Monitor
Shows which job is running on each core:
- **Core 1**: Job ID + current state
- **Core 2**: Job ID + current state

**Visual**: Progress bars showing execution progress.

### 🏦 Banker's Algorithm Status
Real-time deadlock avoidance visualization:
- ✅ **SAFE allocations** (Green): Request granted
- ❌ **DENIED allocations** (Red): Would lead to unsafe state

Shows Solar, Grid, Battery watts for each decision.

### 👥 Process Queue Animation
Color-coded job queue visualization:
- 🔵 **NEW**: Dark (pending)
- 🟠 **READY**: Orange (queued)
- 🟣 **RUNNING**: Purple glow (executing)
- 🟢 **TERMINATED**: Green (completed)

**Interactive**: Hover over any job to see details.

### 📊 Statistics Dashboard
Real-time counters:
- Total events processed
- Jobs created
- Successful completions
- Resource grants/denials
- Simulation duration

## 🖱️ How to Use the Dashboard

### Step 1: Launch Server
```bash
python3 web_server.py
```

### Step 2: Open Dashboard
Go to **http://localhost:5000** in your browser.

### Step 3: Run Simulation
Click **"▶ Run Simulation"** button:
- Simulation runs in C (background)
- Takes ~25-30 seconds
- Dashboard updates in real-time
- All visualizations animate

### Step 4: Analyze Results
Once complete, you can see:
- **Timeline**: Job execution order
- **Resources**: How energy was allocated
- **Banker's**: How many safe vs denied decisions
- **Statistics**: Performance metrics
- **Queue**: How jobs moved through states

### Step 5: Interact
- 🔄 **Reload Data**: Refresh from JSON files
- 🗑 **Clear Data**: Reset for fresh simulation
- Move mouse over elements for details
- Scroll to see full timelines

## 📊 Understanding the Data

### JSON Event Files

**sim_events.json** - Complete simulation events:
```json
{
  "events": [
    {
      "ts": 0,                    // Timestamp (ms)
      "type": "JOB_CREATED",      // Event type
      "job": "JOB_001",           // Job ID
      "core": 0,
      "from": "NEW",              // Previous state
      "to": "READY",              // New state
      "mode": "USER",             // CPU mode
      "sol": 100,                 // Solar available
      "grid": 200,                // Grid available
      "bat": 50                   // Battery available
    },
    ...
  ],
  "count": 247
}
```

**sim_events_live.jsonl** - Line-delimited for streaming:
```
{"ts":0,"type":"JOB_CREATED","job":"JOB_001",...}
{"ts":1,"type":"STATE_TRANSITION","job":"JOB_001",...}
{"ts":500,"type":"RESOURCE_GRANTED","job":"JOB_001",...}
```

## 🔌 API Endpoints

Access simulation data via HTTP:

| Endpoint | Method | Purpose |
|----------|--------|---------|
| `/` | GET | Main dashboard |
| `/api/simulation/status` | GET | Check if running |
| `/api/simulation/run` | POST | Start simulation |
| `/api/simulation/events` | GET | All raw events |
| `/api/simulation/stats` | GET | Aggregated stats |
| `/api/simulation/timeline` | GET | Job timeline |
| `/api/simulation/resources` | GET | Resource history |
| `/api/simulation/clear` | POST | Clear data |

**Example**:
```bash
curl http://localhost:5000/api/simulation/stats | jq .

{
  "jobs_created": 8,
  "resource_denials": 0,
  "resource_grants": 8,
  "successful_jobs": 8,
  "total_duration_ms": 25847,
  "total_events": 64
}
```

## 🛠️ Customization

### Change Number of Jobs
Edit `os_core.c`:
```c
#define TOTAL_JOBS 16  // Change from 8 to 16 (or any number)
```

Recompile:
```bash
gcc -pthread -o os_core os_core.c -Wall -Wextra
```

### Adjust Available Resources
In `os_core.c` main():
```c
bankers_set_available(&bankers_state, 150, 250, 75);  // Solar, Grid, Battery
```

### Modify Dashboard Colors
Edit `templates/dashboard.html` - search for `.gauge-fill`:
```css
.gauge-fill.solar {
    background: linear-gradient(90deg, #fbbf24 0%, #f59e0b 100%);  /* Change colors */
}
```

### Change Auto-Refresh Rate
In `templates/dashboard.html`:
```javascript
setInterval(loadData, 1000);  // Change from 2000ms to 1000ms (or 5000ms)
```

## 🐛 Troubleshooting

| Problem | Solution |
|---------|----------|
| **"Connection refused"** | Flask server not running. Execute `python3 web_server.py` |
| **No events appear** | Click "▶ Run Simulation" to generate new data |
| **Old data showing** | Click "🗑 Clear Data" then run simulation again |
| **Localhost won't load** | Try `127.0.0.1:5000` instead of `localhost:5000` |
| **Animations stutter** | Close other browser tabs/apps, reduce browser tabs |
| **"Port 5000 in use"** | Change port in `web_server.py` line: `app.run(port=5001)` |
| **Compilation error** | Ensure all `.h` files in `os_engine/` directory |

## 📁 File Structure

```
os_engine/
├── os_core.c                    # Main OS simulation (C)
├── simulation_logger.h          # Event logging (C header)
├── bankers_algorithm.h          # Banker's algorithm (C header)
├── PCB.h                        # Process control block (C header)
├── web_server.py                # Flask backend (Python)
├── requirements.txt             # Python dependencies
├── run_dashboard.sh             # Quick start script
├── templates/
│   └── dashboard.html           # Frontend dashboard (HTML/CSS/JS)
├── WEB_DASHBOARD_README.md      # Main documentation
├── SETUP_GUIDE.md               # This file
├── sim_events.json              # Generated simulation events
└── sim_events_live.jsonl        # Live streaming events
```

## 📊 Example Workflow

1. **Terminal 1**: Start server
   ```bash
   python3 web_server.py
   ```

2. **Terminal 2**: Check API
   ```bash
   curl http://localhost:5000/api/simulation/status
   ```

3. **Browser**: Open dashboard
   - Navigate to `http://localhost:5000`
   - See real-time metrics and visualizations

4. **Browser**: Run simulation
   - Click "▶ Run Simulation"
   - Watch animations in real-time
   - See job progression through timeline

5. **Browser**: Analyze results
   - View statistics
   - Check resource allocation
   - Examine Banker's Algorithm decisions

## 💡 Tips & Tricks

### View Raw Events
```bash
# See all events
cat sim_events.json | python3 -m json.tool

# Count events by type
grep -o '"type":"[^"]*"' sim_events.json | sort | uniq -c

# Find specific job
grep 'JOB_003' sim_events.json
```

### Monitor Server Logs
```bash
# In another terminal
tail -f server.log
```

### Manual API Testing
```bash
# Check status
curl http://localhost:5000/api/simulation/status | jq .

# Get stats
curl http://localhost:5000/api/simulation/stats | jq .

# Get timeline
curl http://localhost:5000/api/simulation/timeline | jq .
```

### Profile Different Scenarios

By modifying constants and recompiling, you can test:
- **More jobs**: `#define TOTAL_JOBS 32`
- **Limited resources**: `bankers_set_available(state, 50, 100, 25)`
- **Longer bursts**: `burst_time = (rand() % 10) + 1`
- **More cores**: `#define NUM_CORES 4`

## 📚 Learning Outcomes

Using this dashboard, you'll understand:

1. **Process Scheduling**: How jobs move through states
2. **Resource Management**: Safe vs unsafe allocation decisions
3. **Deadlock Avoidance**: Banker's Algorithm in action
4. **Real-Time Systems**: Event-driven architecture
5. **Web Visualization**: Making complex data intuitive

## 🎓 For Academic Use

This tool is perfect for teaching:
- Operating Systems courses
- Resource allocation algorithms
- Concurrency and synchronization
- Data center scheduling
- Real-time systems

Lecture idea: **"Visualizing OS Complexity"**
- Show traditional CLI output (hard to follow)
- Switch to web dashboard (immediate understanding)

## 🔗 Related Documentation

- [WEB_DASHBOARD_README.md](WEB_DASHBOARD_README.md) - Complete feature reference
- [bankers_algorithm_implementation.md](../docs/bankers_algorithm_implementation.md) - Algorithm details
- [os_core.c](os_core.c) - Source code
- [web_server.py](web_server.py) - Backend implementation
- [templates/dashboard.html](templates/dashboard.html) - Frontend code

## 📝 Next Steps

1. **Run it**: Execute `run_dashboard.sh`
2. **Observe**: Watch the simulation and animations
3. **Experiment**: Modify constants and rerun
4. **Analyze**: Study the JSON events for insights
5. **Extend**: Add new visualizations or charts

---

**Happy visualizing!** 🎉

Built with ❤️ for ECO-CLOUD: Carbon-Aware Data Center Scheduler
