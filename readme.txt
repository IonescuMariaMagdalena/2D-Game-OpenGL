Barbie & Ken in the Land of Ilusions

Barbie & Ken are locked in the mirror land of illusions and are forced to break
their way through the army of fake Barbies and Kens that are summoned to stop them.
You, the player have the task to kill as many clones as possible so that they can 
escape and live their normal lives as they did in the movie.

The mechanics that we implemented for the 2D demo are as follows:
1.The collision for the players and the clones;
-we used the AABB-AABB collision which we found on https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-detection
-when a clone touches one of the characters the game will automatically exit
2.Switching characters;
-we wanted to make this game a two player game but we thought it will be more challangeing if we did it one player 
instead
-by pressing the key F you can switch characters 
-each character will have a special ability in the final build of the game(unfortunately we did not do that right now :/ )
3.The shooting;
-by pressing the key C for Barbie and key N for Ken you can shoot the clones so that they die
-the projectiles are triangles
4.Movement;
-w,a,s,d for barbie
-up,down,left,right for ken
-left shit and right shift for sprinting(x2 speed)
5.Object collecting;
-each object that is on the floor (red and white squares) can be picked up by one of the characters
-we have implemented a tag system so that Barbie can get some items and Ken get the other items 
-the items that are generated for Barbie cannot be picked up by Ken and vice versa(the items will be some sort of health or shields)

We hope you enjoy our project! We look forward for the 3D part!



