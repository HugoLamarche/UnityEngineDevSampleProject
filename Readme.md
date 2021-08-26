# Instructions
- Build NativePlugin_Asteroids\AsteroidsCPP\AsteroidsCPP.sln
- Open Project in Unity
- Play

There is a bunch of variables fun to play with in the editor including :
* ShipControlSpeed
* ShipControlRotationSpeed
* ShipMaxSpeed
* MinAsteroidsSpeed
* MaxAsteroidsSpeed
* FireRate
* BulletLifeSpan
* BulletSpeed

# Game Controls
- R : Restart Game

- Up : Push SpaceShip forward according to its orientation
- Down : Push SpaceShip downward according to its orientation
- Right : Rotate the SpaceShip clockwise
- Left : Rotate the SpaceShip anticlockwise

- Space : Fire

# Challenges

- A way to quickly transfer data between C# and C++ was definitely a challenge, I had never done that before so I did research and found about the Marshal system.
My current implementation isn't ideal but I think it's good enough for the amount of data transfered. In the future I would need to only transfer requests about what data changed instead of sending everything everytime.

- I wanted to have an easy way to add level of destruction for asteroids and I'm glad that I figure an easy way to do so. In the current implementation one could simply add new template level to asteroids and thus add destruction level to the system.

- Pretty early I've decided to transfer the fewest data possible for an asteroid and I've decided to use the NAN number to determine if a Asteroid / Bullet was inactive. I'm happy about the result altough I think that keeping everything into a single array for asteroids forced me to compute too much to find indexes of different types of Asteroids (see Game::GetLevelFromIndex(...) ). If I have opted to have one array for each asteroid level I would have saved this but lose the faculty to add more level of destruction. It would be interesting to invest time to find a good way to have both.

- Early I've found out that I needed to restart Unity to rebuild the native plugin... After doing some research about it I found some solutions but I wanted to focus the time I had on making the actual game so I gave up on this. (Thanks to my Ryzen 5800X CPU for starting Unity quiclky ;) )
