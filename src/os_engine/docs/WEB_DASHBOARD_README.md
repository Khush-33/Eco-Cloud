# ECO-CLOUD OS Engine Web Dashboard

A real-time, animated web visualization for the ECO-CLOUD Operating System Engine with **Banker's Algorithm**, **Resource Allocation**, and **Job Scheduling visualization**.

## Features

✨ **Real-Time Visualizations:**
- 📋 **Job Timeline**: Track each job through its state transitions (NEW → READY → RUNNING → TERMINATED)
- ⚡ **Resource Gauges**: Animated bars showing Solar, Grid, and Battery power allocation
- 🖥️ **CPU Core Monitor**: Visual representation of what each CPU core is running
- 🏦 **Banker's Algorithm Status**: Safe/Unsafe allocation decisions with real-time tracking
- 👥 **Process Queue**: Animated queue showing job states with color-coded indicators
- 📊 **Statistics Dashboard**: Real-time metrics and performance counters

🎨 **High-Quality UI:**
- Modern dark theme with gradient accents
- Smooth animations and transitions
- Responsive design (works on desktop and mobile)
- Intuitive color coding (Solar=Gold, Grid=Blue, Battery=Green)

🚀 **Real-Time Updates:**
- Auto-refreshes every 2 seconds
- Live event streaming from C simulation
- Responsive button states and loading indicators

## Architecture

```
os_core (C Binary)
    ↓ generates JSON events
sim_events.json / sim_events_live.jsonl
    ↓
Flask Web Server (Python)
    ↓ serves API endpoints
React Dashboard (HTML/JS/CSS)
    ↓ displays visualizations
Browser
```

## Installation

### Prerequisites

- Python 3.8+
- Compiled `os_core` executable in the same directory
- Modern web browser (Chrome, Firefox, Safari, Edge)

### Setup

1. **Install Python dependencies:**
```bash
pip install -r requirements.txt
```

2. **Ensure os_core is compiled:**
```bash
gcc -pthread -o os_core os_core.c -Wall -Wextra
```

## Running the Dashboard

### Start the Web Server

```bash
python3 web_server.py
```

The server will start on `http://localhost:5000`

### Using the Dashboard

1. **Open in Browser**: Navigate to `http://localhost:5000`

2. **Run Simulation**: Click "▶ Run Simulation" button
   - Simulation runs in background (takes ~25-30 seconds)
   - Dashboard auto-updates with events
   - All visualizations animate in real-time

3. **Interact:**
   - **🔄 Reload Data**: Manually refresh from JSON files
   - **🗑 Clear Data**: Reset all stored events
   - **Hover** on queue items for more info
   - **Timeline auto-scrolls** as new events arrive

## API Endpoints

All endpoints return JSON data that drives the visualizations:

- `GET /` - Main dashboard HTML
- `GET /api/simulation/status` - Check if simulation is running
- `POST /api/simulation/run` - Start a new simulation
- `GET /api/simulation/events` - Get all raw events
- `GET /api/simulation/stats` - Get aggregated statistics
- `GET /api/simulation/timeline` - Get job timeline view
- `GET /api/simulation/resources` - Get resource allocation timeline
- `POST /api/simulation/clear` - Clear all data

## Understanding the Visualizations

### Timeline View
Shows each job's journey through OS states:
- **NEW** → Created by admission control
- **READY** → Queued and ready to run
- **RUNNING** → Currently executing on a core
- **TERMINATED** → Finished execution

### Resource Allocation
Three energy sources with real-time usage:
- **☀️ Solar**: Renewable energy (0-100W available)
- **🔌 Grid**: Primary grid power (0-200W available)
- **🔋 Battery**: Backup power (0-50W available)

Banker's Algorithm ensures safe allocation - never allocates if it would prevent processes from completing.

### CPU Cores
Displays which job is running on each core with progress indication.
Green bars show execution progress.

### Banker's Algorithm Status
- ✓ **SAFE**: Allocation granted (system remains deadlock-free)
- ✗ **DENIED**: Allocation rejected (would lead to unsafe state)

Shows resource amounts in Watts for each energy type.

### Process Queue
Color-coded job indicators:
- 🔵 **NEW**: Dark blue (pending admission)
- 🟠 **READY**: Orange (waiting in queue)
- 🟣 **RUNNING**: Purple with glow (actively executing)
- 🟢 **TERMINATED**: Green (completed)

## Example Output Files

### sim_events.json
```json
{
  "events": [
    {"ts":0,"type":"JOB_CREATED","job":"JOB_001","core":0,...},
    {"ts":1,"type":"STATE_TRANSITION","job":"JOB_001","from":"NEW","to":"READY",...},
    {"ts":500,"type":"RESOURCE_GRANTED","job":"JOB_001","solar":16,"grid":50,"battery":8,...},
    ...
  ],
  "count": 247
}
```

### sim_events_live.jsonl
One JSON object per line for streaming (used in live updates):
```
{"ts":0,"type":"JOB_CREATED",...}
{"ts":1,"type":"STATE_TRANSITION",...}
{"ts":500,"type":"RESOURCE_GRANTED",...}
```

## Performance Tips

- **Large simulations**: May take 30+ seconds. Dashboard shows progress.
- **Multiple jobs**: More events = slower dashboard (fine for 8-16 jobs)
- **Browser refresh**: Try F5 if visualizations seem stuck
- **Console**: Open browser dev tools (F12) to see any JS errors

## Troubleshooting

| Issue | Solution |
|-------|----------|
| "Connection refused" | Ensure Flask server is running: `python3 web_server.py` |
| No events appear | Click "▶ Run Simulation" or "🔄 Reload Data" |
| Old data showing | Click "🗑 Clear Data" then "▶ Run Simulation" |
| Animations choppy | Close other browser tabs/apps consuming resources |
| Compilation fails | Ensure you're in `os_engine/` directory with all `.h` files |

## Customization

### Change Job Count
Edit `os_core.c`:
```c
#define TOTAL_JOBS 8  // Change this number
```

### Adjust Resources
In `main()`:
```c
bankers_set_available(&bankers_state, 100, 200, 50);  // Solar, Grid, Battery
```

### Modify Update Frequency
In `dashboard.html`, change the interval (in ms):
```javascript
setInterval(loadData, 2000);  // Update every 2 seconds
```

## Technical Details

### Data Flow

1. **C Simulation** generates events with timestamps
2. **Simulation Logger** writes events to JSON files in real-time
3. **Flask Server** reads JSON and provides API endpoints
4. **Dashboard** fetches data every 2 seconds
5. **JavaScript** renders and animates visualizations

### Event Types

- `JOB_CREATED` - New job admission
- `STATE_TRANSITION` - Job changes state
- `MODE_SWITCH` - User ↔ Kernel mode
- `RESOURCE_GRANTED` - Banker's Algorithm granted allocation
- `RESOURCE_DENIED` - Banker's Algorithm denied allocation  
- `RESOURCE_RELEASED` - Job released resources on termination

### Color Scheme

| Component | Color | Meaning |
|-----------|-------|---------|
| Solar | Gold (#fbbf24) | ☀️ Renewable |
| Grid | Blue (#60a5fa) | 🔌 Primary |
| Battery | Green (#4ade80) | 🔋 Backup |
| Safe | Green | ✓ Deadlock-free |
| Unsafe | Red | ✗ Would block |
| Running | Purple | 🟣 Active |

## Files Generated

After running simulation:

- `sim_events.json` - Complete event history (JSON)
- `sim_events_live.jsonl` - Line-delimited JSON for streaming

Both files can be manually inspected or processed with other tools.

## Future Enhancements

- [ ] WebSocket support for true real-time streaming
- [ ] Pause/Resume simulation controls
- [ ] Event playback/scrubbing
- [ ] Export visualizations as images
- [ ] Multiple core/job simulation runs
- [ ] Dark mode toggle
- [ ] Thermal visualization

## License

MIT - Part of ECO-CLOUD project

---

**Built for ECO-CLOUD: Carbon-Aware Data Center Scheduler**
