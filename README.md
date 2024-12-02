Overview:
  This is an implementation of the game Battleship, with both modes Player 1 vs. Player 2 and Player vs. Bot. We have 3 different modes for the Player vs. Bot, easy, medium, or hard, and 2 modes
  for Player 1 vs. Player 2. Both players have access to advanced functions like Smoke Screen, Artillery Strikes, Torpedoes, and Radar Sweep once a ship has been sunk.
  This game allows to player to choose whether or not the grid is displayed (i.e. to choose between the two difficulty levels).

 Features:
    General Gameplay:
      1) Each player has their own 10x10 grid, and the ability to use their functions on the opponents 10x10 grid.
      2) Players can deploy 5 different ships of different sizes (one of size 5 cells, one of size 4 cells, two of size 3 cells, one of size 2 cells) horizontally or vertically.
      3) The players take turns.

    Player Features / Abilities: 
      1) Fire: only fires once a turn on one grid cell.
      2) Radar Sweep: detects a ship within an area of 3x3 based on the coordinate given. With limit of 3 radar sweeps per player.
      3) Smoke Screen: hides an area of 3x3 based on the coordinate given from enemy radar sweeps. With limit of 3 smoke screens per player.
      4) Torpedoes: hits either a whole row or column. Only available after sinking a ship.
      5) Artillery Strikes: hits an entire 3x3 area. Only available after sinking a ship.

    Bot Features / Abilities:
      1) Difficulties:
        - easy: random firing with the occasional radar sweeps and smoke screens.
        - medium: random firing but if a hit was successful (not a miss) targets adjacent spots with the additional occasional torpedoes and artillery strikes when available.
        - hard: strategic targeting, prioritizing least-hit rows/columns, and attacking adjacent cells intelligently.

      2) Advanced weapon use: depending on the difficulty, the bot will choose to either use radar sweeps and smoke screens (if still available) or torpedoes and artillery strikes (upon availability).

    Limitations of actions
    - actions like Radar Sweep, Smoke Screens, Torpedoes, and Artillery have limited availability, tracked per player/bot.
    - availability is tied to gameplay milestones:
        1) Torpedoes and Artillery become available after sinking a ship.
        2) Radar Sweeps and Smoke Screens are limited in number (3 for each player).

    Game setup:
      1) players place their 5 ships on their grid, whether vertically or horizontally. Each ship is named and the player is told how many cells the ship takes hold of.
      2) the bot automatically places its ships randomly.
      3) the game is turn based, and the players are able to use the actions that are available at the time. The bot uses the abilities its able to use based on the difficulty chosen.
      4) all the action names are case sensitive, and must be typed with only the first letter capitalized.
      5) the game ends when all 5 ships of one of the players (whether 2 humans or human vs. bot) have sunk.

    
    
    
