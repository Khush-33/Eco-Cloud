#!/bin/bash
# Quick start script for ECO-CLOUD OS Engine Dashboard

echo "=========================================="
echo "ECO-CLOUD OS Engine Dashboard - Quick Start"
echo "=========================================="

# Check if os_core exists
if [ ! -f "./os_core" ]; then
    echo "❌ os_core executable not found!"
    echo "📦 Compiling os_core.c..."
    gcc -pthread -o os_core os_core.c -Wall -Wextra
    if [ $? -ne 0 ]; then
        echo "❌ Compilation failed!"
        exit 1
    fi
    echo "✓ Compilation successful"
fi

# Install dependencies
echo ""
echo "📦 Installing Python dependencies..."
pip install -q flask flask-cors

# Find an available port (prefer 5000)
PORT=5000
for CANDIDATE in 5000 5001 5002 5003 5004 5005 5006 5007 5008 5009 5010; do
    if ! lsof -i :"$CANDIDATE" -P -n >/dev/null 2>&1; then
        PORT="$CANDIDATE"
        break
    fi
done

# Start the server
echo ""
echo "🚀 Starting ECO-CLOUD Dashboard on http://localhost:${PORT}"
if [ "$PORT" != "5000" ]; then
    echo "ℹ️ Port 5000 is busy, auto-selected port ${PORT}."
fi
echo "---"
echo "Press Ctrl+C to stop the server"
echo "---"
echo ""

PORT="$PORT" python3 web_server.py
