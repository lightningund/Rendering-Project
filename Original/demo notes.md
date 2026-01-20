Changing the column math from three trig functions and a bunch of divides into only one of each, leading to an almost 2x speedup
Figuring out how to update the display in two parts, the render and the map, so they can run independently
Setting up a min heap for the tasks
Optimizing everything a bunch
Creating a benchmarking system to time how long things took so I can prioritize slow functions
Changing the cells to exist in a grid
Limiting the number of cells that are "active" to speed up rendering
Using a `uint32_t` as a bitset for open cell spots
Lowering the speed of the light toggle to every frame instead of twice per frame because it gave me a headache
The way the walls are stored!
	The fact that I have 3 different pieces of information packed into every wall
	And every Cell has one number with 4 walls packed into it
	About how much easier and clearer everything got when I added cell wall helper functions
The way that "fake walls" work
	We generate a cell, decide one of the walls is going to be open
	If we then actually generated the neighboring cell, we would end up generating the whole maze
	Instead, we mark the open wall as *fake*
	When a ray hits a fake wall, we then mark the next cell to be generated as that one
	Only one cell is generated per ray-cast
	If a ray hits something, but then there's a fake wall closer to it, we ignore the fake wall
		This helps prevent the literal corner-case
		This only kind of works
Whenever a cell doesn't get hit by a ray for an entire frame, we just yeet it
	Any neighboring open walls get marked as fake
	I had to make sure that I set all the walls of the yeeted cell to solid otherwise it would gradually get deleted
I also came up with a way to penny pinch storage for the textures, by using a palette!
	Every texture is 16x16, which would be a huge amount of data if I was storing the full color for each pixel
	Instead, each pixel is simply an index into a palette array
	Currently, I have it set to use 4 colors, so 2 bits per index
	I'm using `uint32_t` for the datatype, so I can fit 16 "pixels" into each number!
	This way, I use 16\*32 bits for the actual texture, and another 4\*16 bits for the palette
		That's only 576 bits per texture!
	However, writing each one of those by hand would be a nightmare
	So, I made the sprites in my totally legitimate(tm) copy of aseprite, and saved them as bmp files
	Fun fact! bmp files *also* use a palette system just like this!
	So, I just opened the files in a hex editor, wrote a quick JS script to convert them into the right format, and tada!
	Then I simply had to copy over the colors, and it was all set

Issues:
I have literally no idea how the timing is working
	It is supposed to be running at 10fps, but it seems so much smoother than that
	I individually timed each function, averaged over 10 runs, and the sums of their times should fit within the framerate
	But like... sometimes there's lag and I could not tell you why
I have no idea why walls that are really far away get so bright
With basically minimal effort, I could also make the walls be at any angle, and could give them custom textures instead of solid colors
	Of course, by minimal effort, I mean little to no extra computing
	Plenty of work for me lol

Setting up the actual dev environment was a fucking nightmare
	The WSL install on my laptop is weird because I've messed with it, so there were install issues
	The WSL install on my desktop was more normal, but there was another issue
	Whenever I installed to the device from my desktop, in any way, it just bricked the board
	I eventually solved this by literally wiping the entirety of the flash memory using microchip studio
	But I had to borrow my boyfriend's 2014 Macbook air for the entire term
	I had to download the installers for xcode and the other thing on my desktop and then transfer it over to the mac, because it was too slow
	I had to search through old versions to find one that actually supported the version of OSX that was installed
	I had to figure out a way to disable iCloud because otherwise it would bring the CPU to 95C
	I didn't have my password manager installed obviously, so I had to type my 100 character long github password in on the mac too

My mom used to tell me these stories about a kid named Riley. He lived in this huge mansion, where it seemed that no place was ever the same twice. Hallways whose end seemed to get further away as you walked, rooms that move around the house, pathways that branch on and on seemingly forever. You could leave a room and come back to find it completely gone. I always loved hearing about the crazy things Riley would see in the mansion, and I think it was part of what inspired the idea for this project; an ever-shifting labyrinth that literally changes every time you look at it.