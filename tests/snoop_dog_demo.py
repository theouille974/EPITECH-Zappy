#!/usr/bin/env python3
"""
Quick demo that spawns a SNOOP_DOG team so you can visually verify the Easter-egg
(Snoop Dogg face texture + green colour).

It reuses the three compiled binaries:
    • zappy_server (C)
    • zappy_gui    (C++)
    • zappy_ia     (Python)

The world is small (10×10) and runs at low frequency so you can observe easily.
Press Ctrl-C to exit – the script cleans up all child processes.
"""

import subprocess
import time
import os
import sys
from pathlib import Path

BIN_SERVER = Path(__file__).resolve().parent.parent / "zappy_server"
BIN_GUI    = Path(__file__).resolve().parent.parent / "zappy_gui"
BIN_IA     = Path(__file__).resolve().parent.parent / "zappy_ia"

TEAM_SNOOP = "SNOOP_DOG"
OTHER_TEAM = "RED"  # just to have a second team

PORT = "4242"
HOST = "localhost"


def exists_or_die(path: Path):
    if not path.exists():
        print(f"❌ Required binary {path} not found – build the project first.")
        sys.exit(1)


def spawn(cmd, **kwargs):
    """Helper wrapping Popen that prints the command."""
    print("$ ", " ".join(map(str, cmd)))
    return subprocess.Popen(cmd, **kwargs)


def start_server():
    print("🚀 Starting Zappy server…")
    return spawn([
        str(BIN_SERVER),
        "-p", PORT,
        "-x", "10", "-y", "10",
        "-n", TEAM_SNOOP, OTHER_TEAM,
        "-c", "8",
        "-f", "10"
    ], stdout=subprocess.PIPE, stderr=subprocess.PIPE)


def start_gui():
    print("🖥️  Starting GUI…")
    return spawn([str(BIN_GUI), "-p", PORT, "-h", HOST])


def start_ai(team: str, idx: int):
    print(f"🤖 Starting AI {team} #{idx}…")
    return spawn([
        str(BIN_IA),
        "-p", PORT,
        "-n", team,
        "-h", HOST,
        "--quiet"
    ], stdout=subprocess.PIPE, stderr=subprocess.PIPE)


def main():
    for binary in (BIN_SERVER, BIN_GUI, BIN_IA):
        exists_or_die(binary)

    server = None
    bots   = []

    try:
        server = start_server()
        time.sleep(2)

        # Launch multiple Snoop bots and other team bots for better testing
        print("🤖 Connecting multiple bots...")
        
        # SNOOP_DOG bots
        bots.append(start_ai(TEAM_SNOOP, 1))
        time.sleep(0.3)
        bots.append(start_ai(TEAM_SNOOP, 2))
        time.sleep(0.3)
        bots.append(start_ai(TEAM_SNOOP, 3))
        time.sleep(0.3)
        bots.append(start_ai(TEAM_SNOOP, 4))
        time.sleep(0.3)
        
        # Other team bots for contrast
        bots.append(start_ai(OTHER_TEAM, 1))
        time.sleep(0.3)
        bots.append(start_ai(OTHER_TEAM, 2))
        time.sleep(0.3)
        bots.append(start_ai(OTHER_TEAM, 3))
        time.sleep(0.3)

        print("✅ Server running and bots connected!")
        print(f"   - {TEAM_SNOOP}: 4 bots")
        print(f"   - {OTHER_TEAM}: 3 bots")
        print("💡 Launch the GUI yourself in another terminal:")
        print("   ./zappy_gui -p 4242 -h localhost")
        print("Then look for green Snoop faces and listen for snoop music.")
        print("Press Ctrl+C here to stop the demo…")
        while True:
            time.sleep(1)

    except KeyboardInterrupt:
        print("\n🛑 Stopping demo…")
    finally:
        for p in bots:
            if p: p.terminate()
        if server: server.terminate()
        # Give processes a moment to exit
        time.sleep(1)
        print("✅ Clean exit.")


if __name__ == "__main__":
    main() 