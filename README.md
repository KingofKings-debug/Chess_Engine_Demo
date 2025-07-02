# ♟️ Chess Project (Demo Version)

> ⚠️ **Note:** This is just a demo version. A full-featured, proper chess implementation is under development and will be available in a separate repository soon.

---

## 🖼️ Overview

- Uses the **SFML** library for graphics rendering.
- Images for chess pieces are stored in separate folders:
  - `white/` for white pieces  
  - `black/` for black pieces

---

## 📐 Board Representation

- The board is implemented as a **1D array of 64 integers**.
- **White pieces** are represented by **positive integers**.
- **Black pieces** are represented by **negative integers**.
- **Empty squares** are represented by `0`.

---

## 🤖 AI & Gameplay

- The **chessAI** script currently plays as **white**.
- It uses a "Minimax algorithm" to determine the best move.
- After the AI moves, the **player (black)** can make a move.
- After the player's move, **click again** to let the AI perform its next move.

---

## 🔄 Move Generation

- Board positions (`0–63`) are first converted into `(posX, posY)` format.
- All legal moves are generated using this 2D coordinate system.
- Moves are then converted back to single-number positions for execution.


---
![image](https://github.com/user-attachments/assets/8a938b71-8244-4838-89e8-ee561633428e)




