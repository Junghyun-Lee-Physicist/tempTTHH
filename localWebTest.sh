#!/bin/bash
# Start a local HTTP server for testing the config dashboard
# Open http://localhost:8000 in your browser
echo "Starting local server at http://localhost:8000"
python3 -m http.server 8000
