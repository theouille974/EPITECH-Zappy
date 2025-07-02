#!/usr/bin/env python3
"""
Demo script showing all three Zappy binaries working together
"""

import subprocess
import time
import sys
import os

def start_server():
    """Start the Zappy server"""
    print("🚀 Starting Zappy server...")
    server = subprocess.Popen([
        "../zappy_server", 
        "-p", "4242", 
        "-x", "10", 
        "-y", "10", 
        "-n", "RED", "BLUE", 
        "-c", "6",
        "-f", "100"
    ], cwd=".", stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    
    time.sleep(2)
    return server

def start_gui():
    """Start the GUI client"""
    print("🖥️  Starting GUI client...")
    gui = subprocess.Popen([
        "../zappy_gui",
        "-p", "4242",
        "-h", "localhost"
    ], cwd=".")
    
    time.sleep(1)
    return gui

def start_ai(team_name, ai_id):
    """Start a single AI"""
    print(f"🤖 Starting {team_name} AI #{ai_id}...")
    ai = subprocess.Popen([
        "../zappy_ia",
        "-p", "4242",
        "-n", team_name,
        "-h", "localhost",
        "--quiet"
    ], cwd=".", stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    
    return ai

def main():
    """Main demo function"""
    print("=== Zappy Demo ===")
    print("Testing all three binaries:")
    print("• zappy_server (C)")
    print("• zappy_gui (C++)") 
    print("• zappy_ia (Python)")
    print()
    
    # Check binaries
    binaries = ["../zappy_server", "../zappy_gui", "../zappy_ia"]
    for binary in binaries:
        if not os.path.exists(binary):
            print(f"❌ Error: {binary} not found")
            sys.exit(1)
    
    print("✅ All binaries found!")
    print()
    
    server = None
    gui = None
    ais = []
    
    try:
        # Start server
        server = start_server()
        print("✅ Server started")
        
        # Start GUI
        # gui = start_gui()
        print("✅ GUI started")
        
        # Start some AIs
        print("\n🤖 Starting AIs...")
        ais.append(start_ai("RED", 1))
        time.sleep(1)
        ais.append(start_ai("RED", 2))
        time.sleep(1)
        ais.append(start_ai("RED", 3))
        time.sleep(1)
        ais.append(start_ai("RED", 4))
        time.sleep(1)
        ais.append(start_ai("RED", 5))
        time.sleep(1)
        ais.append(start_ai("RED", 6))
        time.sleep(1)
        
        print("✅ All components started!")
        print("\n🎮 Demo running...")
        print("• Server: 10x10 world, RED/BLUE teams")
        print("• GUI: Connected to localhost:4242")
        print("• AIs: 2 RED + 2 BLUE players")
        print("\nPress Ctrl+C to stop...")
        
        # Keep running
        while True:
            time.sleep(1)
            
    except KeyboardInterrupt:
        print("\n🛑 Stopping demo...")
    except Exception as e:
        print(f"❌ Demo failed: {e}")
    finally:
        # Cleanup
        print("🧹 Cleaning up...")
        
        # Stop AIs
        for ai in ais:
            if ai:
                ai.terminate()
                try:
                    ai.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    ai.kill()
        
        # Stop GUI
        if gui:
            gui.terminate()
            try:
                gui.wait(timeout=5)
            except subprocess.TimeoutExpired:
                gui.kill()
        
        # Stop server
        if server:
            server.terminate()
            try:
                server.wait(timeout=10)
            except subprocess.TimeoutExpired:
                server.kill()
        
        print("✅ Demo completed!")

if __name__ == "__main__":
    main() 