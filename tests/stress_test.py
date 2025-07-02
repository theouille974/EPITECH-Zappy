#!/usr/bin/env python3
"""
Interactive stress test script for Zappy server
Runs a 42x42 server with RED and BLUE teams, connects 21 AIs per team
You can manually launch the GUI to watch the action!
"""

import subprocess
import socket
import time
import threading
import sys
import os
import signal

def start_server():
    """Start the Zappy server with specified parameters"""
    print("🚀 Starting Zappy server...")
    server = subprocess.Popen([
        "../zappy_server", 
        "-p", "4242", 
        "-x", "42", 
        "-y", "42", 
        "-n", "RED", "BLUE", 
        "-c", "42", 
        "-f", "10"
    ], cwd=".", stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    
    # Wait for server to start
    time.sleep(3)
    return server

def stop_server(server):
    """Stop the server gracefully"""
    print("🛑 Stopping server...")
    server.terminate()
    try:
        server.wait(timeout=10)
    except subprocess.TimeoutExpired:
        server.kill()
        server.wait()

def start_ai(team_name, ai_id):
    """Start a single AI process"""
    try:
        ai = subprocess.Popen([
            "../zappy_ia",
            "-p", "4242",
            "-n", team_name,
            "-h", "localhost",
            "--quiet"
        ], cwd=".", stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        
        print(f"🤖 AI {ai_id} ({team_name}): Started")
        return ai
        
    except Exception as e:
        print(f"❌ AI {ai_id} ({team_name}): Failed to start - {e}")
        return None

def connect_team_ais(team_name, count):
    """Start multiple AIs for a team"""
    ais = []
    
    print(f"🤖 Starting {count} {team_name} AIs...")
    
    for i in range(count):
        ai = start_ai(team_name, i + 1)
        if ai:
            ais.append(ai)
        else:
            print(f"❌ Failed to start {team_name} AI {i + 1}")
        
        # Small delay between starts
        time.sleep(0.3)
    
    print(f"✅ Successfully started {len(ais)} {team_name} AIs")
    return ais

def signal_handler(signum, frame):
    """Handle Ctrl+C gracefully"""
    print("\n🛑 Received interrupt signal. Shutting down...")
    sys.exit(0)

def main():
    """Main test function"""
    print("=== Zappy Interactive Stress Test ===")
    print("Server: 42x42 board, RED/BLUE teams, 42 capacity each")
    print("AIs: 21 RED + 21 BLUE = 42 total")
    print("GUI: Launch manually with: ./zappy_gui -p 4242 -h localhost")
    print()
    
    # Set up signal handler for graceful shutdown
    signal.signal(signal.SIGINT, signal_handler)
    
    # Check if binaries exist
    if not os.path.exists("../zappy_server"):
        print("❌ Error: zappy_server binary not found")
        print("Please build the server first")
        sys.exit(1)
        
    if not os.path.exists("../zappy_ia"):
        print("❌ Error: zappy_ia binary not found")
        print("Please build the AI client first")
        sys.exit(1)
    
    server = None
    red_ais = []
    blue_ais = []
    
    try:
        # Start server
        server = start_server()
        print("✅ Server started successfully")
        
        # Start RED team AIs
        red_ais = connect_team_ais("RED", 21)
        
        # Start BLUE team AIs  
        blue_ais = connect_team_ais("BLUE", 21)
        
        total_started = len(red_ais) + len(blue_ais)
        print(f"\n📊 Total AIs started: {total_started}/42")
        
        if total_started == 42:
            print("🎉 SUCCESS: All 42 AIs started!")
        else:
            print(f"⚠️  WARNING: Only {total_started}/42 AIs started")
        
        print("\n🎮 Interactive stress test is now running!")
        print("• Server: 42x42 world with RED/BLUE teams")
        print("• AIs: 21 RED + 21 BLUE players active")
        print("• Duration: Run as long as you want")
        print("\n💡 To watch the action:")
        print("  • Open a new terminal")
        print("  • Run: ./zappy_gui -p 4242 -h localhost")
        print("  • Watch the beautiful animations!")
        print("\n⏰ Test started at:", time.strftime("%H:%M:%S"))
        print("🔄 Running indefinitely...")
        print("🛑 Press Ctrl+C to stop")
        
        # Keep running indefinitely
        while True:
            time.sleep(1)
            
    except KeyboardInterrupt:
        print("\n🛑 Test interrupted by user")
    except Exception as e:
        print(f"❌ Test failed: {e}")
    finally:
        # Cleanup
        print("🧹 Cleaning up...")
        
        # Stop all AIs
        print("🤖 Stopping AIs...")
        for ai in red_ais + blue_ais:
            if ai:
                ai.terminate()
                try:
                    ai.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    ai.kill()
        
        # Stop server
        if server:
            stop_server(server)
        
        print("✅ Cleanup completed!")
        print("⏰ Test ended at:", time.strftime("%H:%M:%S"))

if __name__ == "__main__":
    main() 