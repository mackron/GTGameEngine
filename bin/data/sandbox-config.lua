-- This script will be executed when the sandbox is started. You can execute your own scripts by calling
-- GTEngine.ExecuteFile("my-script-file"), or simply adding it to the end of this file.
--
-- These scripts are executed on the main game script which means your game objects can access any properties
-- declared here whenever they are needed.
--
-- This is executed as a standard Lua script. You can use dynamic logic here like branching and loops.

--Game.StartupScene = "";
Game.StartupScene = "sandbox/intro.gtscene";        -- The relative path of the initial scene to load. This is relative to the data directory. If this is not a valid scene, the editor will be opened.
Game.Title        = "GTGameEngine Sandbox";         -- The title to show on the game window.
Game.Width        = 1280;                           -- The horiziontal resolution.
Game.Height       = 720;                            -- The vertical resolution.
Game.Fullscreen   = false;                          -- Whether or not to start in fullscreen mode.
Game.EnableVSync  = false;                          -- Whether or not to enable v-sync.
Game.AspectRatio  = 0;                              -- If set to 0, the aspect ratio will be re-calculated whenever the game window is resized as the window width divided by height.
--Game.AspectRatio  = Game.Width / Game.Height;       -- Use this to use a constant aspect ratio. Might also like to use "16 / 9", "4 / 3", etc (without quotes).

Game.CollisionGroups.Static  = 1;
Game.CollisionGroups.Dynamic = 2;
Game.CollisionGroups.Player  = 3;
Game.CollisionGroups.Bullet  = 4;
