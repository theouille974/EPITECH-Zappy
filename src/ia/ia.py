import socket
import time
import argparse
import random
import sys

TEAM_SECRET = "zappy_rocks"

class ZappyIA:
    ELEVATION_REQUIREMENTS = [
        None,
        {"player": 1, "linemate": 1, "deraumere": 0, "sibur": 0, "mendiane": 0, "phiras": 0, "thystame": 0},  # 1 -> 2
        {"player": 2, "linemate": 1, "deraumere": 1, "sibur": 1, "mendiane": 0, "phiras": 0, "thystame": 0},  # 2 -> 3
        {"player": 2, "linemate": 2, "deraumere": 0, "sibur": 1, "mendiane": 0, "phiras": 2, "thystame": 0},  # 3 -> 4
        {"player": 4, "linemate": 1, "deraumere": 1, "sibur": 2, "mendiane": 0, "phiras": 1, "thystame": 0},  # 4 -> 5
        {"player": 4, "linemate": 1, "deraumere": 2, "sibur": 1, "mendiane": 3, "phiras": 0, "thystame": 0},  # 5 -> 6
        {"player": 6, "linemate": 1, "deraumere": 2, "sibur": 3, "mendiane": 0, "phiras": 1, "thystame": 0},  # 6 -> 7
        {"player": 6, "linemate": 2, "deraumere": 2, "sibur": 2, "mendiane": 2, "phiras": 2, "thystame": 1},  # 7 -> 8
    ]

    def __init__(self, team="team1", host="localhost", port=4242, verbose=True):
        self.verbose = verbose
        self.team = team
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((host, port))
        self._log = (lambda *a, **k: print(*a, **k)) if verbose else (lambda *a, **k: None)
        self.level = 1
        self.inventory_cache = {}
        self.state = "EXPLORING"
        self.message_queue = []

        self.sock.settimeout(5)
        welcome = self._recv_line()
        self._log("[IA] Reçu:", welcome)
        if welcome.strip().upper() != "WELCOME":
            raise RuntimeError(f"Unexpected greeting from server: {welcome!r}")

        self._send_line(team)
        self.slots_left = int(self._read_response())
        dims = self._read_response()
        self.map_width, self.map_height = map(int, dims.split())
        self._log(f"[IA] Slots left: {self.slots_left} | Map: {self.map_width}x{self.map_height}")
        self.sock.settimeout(0.1)

    def _send_line(self, msg: str):
        self._log(f"[IA] Envoi: {msg}")
        self.sock.sendall((msg + "\n").encode())

    def _recv_line(self) -> str:
        buf = b""
        while b"\n" not in buf:
            chunk = self.sock.recv(1024)
            if not chunk:
                raise ConnectionError("Connection closed by server")
            buf += chunk
        line, _, rest = buf.partition(b"\n")
        if rest:
            self._pending = getattr(self, "_pending", b"") + rest
        return line.decode().strip()

    def _read_response(self) -> str:
        if hasattr(self, "_pending") and b"\n" in self._pending:
            line, _, self._pending = self._pending.partition(b"\n")
            return line.decode().strip()
        return self._recv_line()

    def cmd(self, command: str) -> str:
        """Send *command* to the server and wait for the associated single-line answer."""
        self._send_line(command)
        try:
            response = self._read_response()
        except socket.timeout:
            self._log("[IA] Timeout: aucune réponse")
            response = ""
        if response == "dead":
            raise SystemExit("[IA] You are dead :(")
        self._log(f"[IA] Reçu: {response}")
        return response

    def forward(self):
        return self.cmd("Forward")

    def right(self):
        return self.cmd("Right")

    def left(self):
        return self.cmd("Left")

    def look(self):
        return self.cmd("Look")

    def inventory(self):
        return self.cmd("Inventory")

    def broadcast(self, text: str):
        return self.cmd(f"Broadcast {text}")

    def connect_nbr(self):
        return self.cmd("Connect_nbr")

    def fork(self):
        return self.cmd("Fork")

    def incantation(self):
        return self.cmd("Incantation")

    def take(self, obj: str):
        return self.cmd(f"Take {obj}")

    def set(self, obj: str):
        return self.cmd(f"Set {obj}")

    def _update_inventory(self):
        """Update the internal inventory cache."""
        inv_raw = self.inventory()
        self.inventory_cache = self._parse_inventory(inv_raw)
        self._log(f"[IA] Inventory: {self.inventory_cache}")

    def _needs_to_forage(self):
        """Check if the AI needs to find food."""
        food_stock = self.inventory_cache.get("food", 0)
        return food_stock < 10

    def _get_needed_stones(self):
        """Get a dict of stones needed for the next elevation."""
        if self.level >= 8:
            return {}
        
        reqs = self.ELEVATION_REQUIREMENTS[self.level].copy()
        needed = {}

        for stone, required_count in reqs.items():
            if stone == "player":
                continue
            
            current_count = self.inventory_cache.get(stone, 0)
            if current_count < required_count:
                needed[stone] = required_count - current_count
        
        return needed

    def _has_required_stones_for_elevation(self):
        return not self._get_needed_stones()

    def _go_to_tile(self, tile_index):
        """Move towards a tile index from the 'look' command output."""
        if tile_index == 0:
            return

        level = 1
        
        row = 0
        tile_cursor = 0
        while tile_cursor < tile_index:
            row += 1
            tile_cursor += (2 * row -1)
        
        moves = []
        if tile_index in [1, 2, 3]:
            moves.append(self.forward)
            if tile_index == 1: moves.append(self.left)
            if tile_index == 3: moves.append(self.right)
        elif tile_index in [4, 5, 6, 7, 8]:
            moves.append(self.forward)
            moves.append(self.forward)
            if tile_index == 4:
                moves.append(self.left)
                moves.append(self.forward)
                moves.append(self.forward)
            elif tile_index == 5:
                moves.append(self.left)
                moves.append(self.forward)
            elif tile_index == 7:
                moves.append(self.right)
                moves.append(self.forward)
            elif tile_index == 8:
                moves.append(self.right)
                moves.append(self.forward)
                moves.append(self.forward)
        
        if not moves:
            self._log(f"[IA] No specific move sequence for tile {tile_index}, moving forward.")
            moves.append(self.forward)

        for move in moves:
            move()
            time.sleep(0.1)

    def _search_for_item(self, item_name):
        """Look for a specific item and move towards it."""
        self._log(f"[IA] Searching for {item_name}...")
        look_raw = self.look()
        tiles = self._parse_look(look_raw)

        for i, tile_content in enumerate(tiles):
            if item_name in tile_content:
                if i == 0:
                    self.take(item_name)
                    self._update_inventory()
                else:
                    self._log(f"[IA] Found {item_name} on tile {i}, moving towards it.")
                    self._go_to_tile(i)
                return True

        self._log(f"[IA] {item_name} not in view, exploring...")
        random.choice([self.left, self.right, self.forward])()
        return False

    def _check_for_messages(self):
        """Check for and queue broadcast messages."""
        try:
            response = self._read_response()
            if response.startswith("message"):
                self.message_queue.append(response)
                self._log(f"[IA] Queued message: {response}")
        except socket.timeout:
            pass

    def _await_incantation_result(self, timeout: float = 10.0) -> str:
        """Wait up to *timeout* seconds for either a success ("Current level")
        or a failure ("ko") message related to an ongoing incantation.

        Returns the received line (may be "" if nothing arrived).
        """
        end = time.time() + timeout
        while time.time() < end:
            try:
                line = self._read_response()
            except socket.timeout:
                time.sleep(0.05)
                continue

            if line.startswith("Current level") or line == "ko":
                return line
            if line.startswith("message"):
                self.message_queue.append(line)
            else:
                self._pending = getattr(self, "_pending", b"") + (line + "\n").encode()
        return ""

    def run(self):
        """State-driven autonomous loop with communication."""
        try:
            self._update_inventory()
            while True:
                self._check_for_messages()
                self._update_inventory()

                if self.state == "GOING_TO_INCANTATION":
                    if not self.message_queue:
                        self._log("[IA] Leader stopped broadcasting. Returning to EXPLORING.")
                        self.state = "EXPLORING"
                        continue
                    
                    msg = self.message_queue.pop(0)
                    parts = msg.split(',')
                    direction = int(parts[0].split()[1])
                    
                    if direction == 0:
                        self._log("[IA] Arrived at incantation point. Waiting.")
                        time.sleep(1)
                        continue

                    self._log(f"[IA] Following call from direction {direction}")
                    if direction in [1, 2, 8]: self.forward()
                    elif direction in [3, 4]: self.left()
                    elif direction in [6, 7]: self.right()
                    else: self.left(); self.left()
                    continue

                if self.message_queue:
                    msg = self.message_queue.pop(0)
                    try:
                        _, content = msg.split(',', 1)
                        secret, level_str, action = content.strip().split(':')
                        if secret == TEAM_SECRET and action == "INCANTATION_START" and int(level_str) == self.level:
                             self.state = "GOING_TO_INCANTATION"
                             self._log(f"[IA] Received call for level {self.level} incantation. Changing state.")
                             continue
                    except (ValueError, IndexError) as e:
                        self._log(f"[IA] Could not parse broadcast: {msg} ({e})")

                if self.level >= 2 and self.inventory_cache.get("food", 0) > 20:
                    if int(self.connect_nbr()) == 0:
                        self._log("[IA] State: FORKING")
                        self.fork()
                        continue

                if self._needs_to_forage():
                    self.state = "GATHERING"
                    self._log("[IA] State: FORAGING (low food)")
                    self._search_for_item("food")
                    continue

                needed_stones = self._get_needed_stones()

                if not needed_stones:
                    self.state = "CALLING_FOR_INCANTATION"
                    self._log("[IA] State: LEVELING UP")
                    reqs = self.ELEVATION_REQUIREMENTS[self.level]
                    
                    on_tile_str = self._parse_look(self.look())[0]
                    on_tile_items = on_tile_str.split()
                    
                    tile_is_clear = True
                    required_stones_for_check = {stone for stone, count in reqs.items() if count > 0 and stone != "player"}
                    
                    for item in on_tile_items:
                        if item == "player" or item == "food": continue
                        if item not in required_stones_for_check:
                            self._log(f"[IA] Tile not clear. Found extra item: {item}. Picking it up.")
                            self.take(item)
                            tile_is_clear = False
                    
                    if not tile_is_clear:
                        time.sleep(0.1)
                        continue

                    players_on_tile = on_tile_items.count("player")
                    
                    if players_on_tile >= reqs["player"]:
                        self._log("[IA] Attempting incantation with players present.")
                        for stone, count in reqs.items():
                            if stone == "player": continue
                            for _ in range(count): self.set(stone)

                        first = self.incantation()

                        if first.lower().startswith("elevation underway"):
                            result = self._await_incantation_result()
                        else:
                            result = first

                        if result.startswith("Current level"): 
                            try:
                                self.level = int(result.split()[-1])
                            except (ValueError, IndexError):
                                self.level += 1
                            self._log(f"[IA] Leveled up to {self.level}!")
                            self.state = "EXPLORING"
                        else:
                            self._log("[IA] Incantation failed. Picking stones back up.")
                            for stone, count in reqs.items():
                                if stone == "player": continue
                                for _ in range(count): self.take(stone)
                    else:
                        self._log(f"[IA] Calling for help to level up. Found {players_on_tile}/{reqs['player']}.")
                        self.broadcast(f"{TEAM_SECRET}:{self.level}:INCANTATION_START")
                        time.sleep(0.5)

                else:
                    self.state = "GATHERING"
                    self._log(f"[IA] State: GATHERING (need {needed_stones})")
                    stone_to_find = list(needed_stones.keys())[0]
                    self._search_for_item(stone_to_find)

                time.sleep(0.1)
        finally:
            self.close()

    def close(self):
        self.sock.close()

    @staticmethod
    def _parse_inventory(raw: str) -> dict:
        """Return a dict of {resource: count} from an Inventory response."""
        raw = raw.strip().lstrip("[").rstrip("]")
        items = [token.strip() for token in raw.split(',') if token.strip()]
        out = {}
        for token in items:
            parts = token.split()
            if len(parts) == 2 and parts[1].isdigit():
                out[parts[0]] = int(parts[1])
        return out

    @staticmethod
    def _parse_look(raw: str) -> list:
        """Return a list of tile strings (already lower-cased) from a Look response."""
        raw = raw.strip().lstrip("[").rstrip("]")
        tiles = [t.strip().lower() for t in raw.split(',')]
        return tiles

    def _forage(self):
        """Look for food in the immediate field of view and act accordingly."""
        look_raw = self.look()
        tiles = self._parse_look(look_raw)
        if "food" in tiles[0]:
            while True:
                resp = self.take("food")
                if resp.lower() == "ko":
                    break
            self._update_inventory()
            return

        if any("food" in t for t in tiles[1:]):
            self.forward()
        else:
            random.choice([self.left, self.right])()

def _parse_cli():
    parser = argparse.ArgumentParser(description="Zappy AI client", add_help=False)

    parser.add_argument("-p", dest="port", type=int, required=True, help="Port number")
    parser.add_argument("-n", dest="team", required=True, help="Team name")
    parser.add_argument("-h", dest="host", default="localhost", help="Server hostname (default: localhost)")

    parser.add_argument("--quiet", action="store_false", dest="verbose", default=True, help="Mute client logs")

    parser.add_argument("--help", "-help", action="help", help="Show this help message and exit")

    return parser.parse_args()

if __name__ == "__main__":
    if len(sys.argv) > 1:
        args = _parse_cli()
        ai = ZappyIA(team=args.team, host=args.host, port=args.port, verbose=args.verbose)
    else:
        ai = ZappyIA(team="team1", verbose=True)

    ai.run()