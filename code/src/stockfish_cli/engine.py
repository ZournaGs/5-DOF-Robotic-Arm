import subprocess
import chess


stockfish_path = "/usr/local/bin/stockfish"

engine = subprocess.Popen(
    stockfish_path,
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    text=True
)

board = chess.Board()

def send(command):
    engine.stdin.write(command + "\n")
    engine.stdin.flush()

def read_until(keyword):
    while True:
        line = engine.stdout.readline().strip()
        if line.startswith(keyword):
            return line

def get_bestmove_from_current_position():
    send(f"position fen {board.fen()}")
    send("go depth 10")

    bestmove_line = read_until("bestmove")
    return bestmove_line.split()[1]

def clear_answer():
    with open("code/src/stockfish_cli/answer.txt","w") as f:
         f.write("")

send("uci")
read_until("uciok")

send("isready")
read_until("readyok")

clear_answer()
try:
 while True:
    user_move = input("Enter move: ")

    
    if len(user_move)<4 or len(user_move)>5:
        print("The move must be 4 or 5 characters long. Please enter again a valid move..")
        continue

    move = chess.Move.from_uci(user_move)

    if move not in board.legal_moves:
        print("Illegal move")
        continue

    board.push(move)

    stockfish_move = get_bestmove_from_current_position()
    print("Stockfish answer:", stockfish_move)

    board.push(chess.Move.from_uci(stockfish_move))
    with open("code/src/stockfish_cli/answer.txt","w") as f:
        f.write(stockfish_move)

except KeyboardInterrupt:
    print("Shutting engine down...")
    clear_answer()

