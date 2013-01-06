#include <sifteo.h>
#include "assets.gen.h"

#define MINNUMBER 0
#define MAXNUMBER 9

using namespace Sifteo;

static Metadata M = Metadata()
    .title("Number Smash")
    .package("com.hackerlab.numbersmash", "0.1")
    .icon(Icon)
    .cubeRange(0, CUBE_ALLOCATION);

static AssetSlot MainSlot = AssetSlot::allocate()
    .bootstrap(GameAssets);

static VideoBuffer vid[CUBE_ALLOCATION];
static TiltShakeRecognizer motion[CUBE_ALLOCATION];

class NumberSmash {
public:
    struct Counter {
        unsigned neighborAdd;
        unsigned neighborRemove;
		unsigned score;
		unsigned currentNumber;
                bool touchOccured;
    } counters[CUBE_ALLOCATION];

     int frame;
     
    void setup()
    {
	Events::neighborAdd.set(&NumberSmash::onNeighborAdd, this);
        Events::neighborRemove.set(&NumberSmash::onNeighborRemove, this);
        Events::cubeAccelChange.set(&NumberSmash::onAccelChange, this);
        Events::cubeTouch.set(&NumberSmash::onTouch, this);
        Events::cubeConnect.set(&NumberSmash::onConnect, this);

        // Handle already-connected cubes
        for (CubeID cube : CubeSet::connected())
            onConnect(cube);
    }

	NumberSmash() {
		random = Random::Random();

	}
    void drawCard(unsigned id)
    {
        CubeID cube(id);
        vid[cube].bg1.image(vec(4,4), Card, counters[id].currentNumber);
       
    }

private:
	Random random;
       
	void playSfx(const AssetAudio& sfx) {
		static int i=0;
		AudioChannel(i).play(sfx);
		i = 1 - i;
	}

    void onConnect(unsigned id)
    {
        CubeID cube(id);

        bzero(counters[id]);
        LOG("Piece %d connected\n", id);
        
       // vid[id].initMode(BG0_ROM);
        vid[id].initMode(BG0_BG1);
        vid[id].attach(id);
        motion[id].attach(id);

        // Draw the cube's identity
        String<128> str;
		
		if(id == 0)
			str << "Center Stack\n";
		else
	        str << "Player #" << cube << "\n";

		counters[id].currentNumber = random.randint(MINNUMBER, MAXNUMBER);
        vid[cube].bg0.image(vec(0,0), Background);
        // Allocate 16x2 tiles on BG1 for text at the bottom of the screen
        auto mask = BG1Mask::filled(vec(2,2), vec(12,12));
        mask = mask | BG1Mask::filled(vec(2,2), vec(12,12));
        vid[cube].bg1.setMask(mask);
        //vid[cube].bg0rom.text(vec(1,2), str);
        vid[cube].bg1.fill(TTile);

        onAccelChange(cube);
        onTouch(cube);
        changeCard(cube);
        
    }

	void displayScore(unsigned id)
	{
        CubeID cube(id);
		String<32> str;
		
		if(id != 0)
			str << "score: " << counters[id].score << "\n";

        //vid[cube].bg0_rom.text(vec(2,2), Font3, str, '!');
       vid[cube].bg1.text(vec(2,2),Font, str);
       // vid.sprites[0].setImage(CardOne, frame);
	}

	void displayNumber(unsigned id)
	{
                CubeID cube(id);
		String<32> str;
		str << "number: " << counters[id].currentNumber << "\n";
                frame = counters[id].currentNumber;
                vid[cube].bg1.text(vec(2,12), Font, str);
                drawCard(id);
	}
        
        void changeCard(unsigned id)
        {
                counters[id].currentNumber = random.randint(MINNUMBER, MAXNUMBER);

		displayNumber(id);

        //str << "touch: " << cube.isTouching() << "\n";
                drawCard(id);
		displayScore(id);
            
            
        }
        

	void onTouch(unsigned id)
    {
		CubeID cube(id);
		/*
                playSfx(SfxBomb);
	

		LOG("Touch event on cube #%d, state=%d\n", id, cube.isTouching());
		//counters[id].touchOccured = true;

		if(!cube.isTouching())
                {
                    counters[id].currentNumber = random.randint(MINNUMBER, MAXNUMBER);
                  //  counters[id].touchOccured = false;
                }
			

*/

		//displayNumber(id);

        //str << "touch: " << cube.isTouching() << "\n";
                drawCard(id);
		displayScore(id);

		
	
    }

    void onAccelChange(unsigned id)
    {
        CubeID cube(id);
        String<64> str;

        if (motion[id].update() && motion[id].shake) {
            //str << "shake: " << motion[id].shake;

			if(id == 0)
				LOG("Reset Game\n");
			else
                        {
				LOG("player %d shook\n", id);
                                changeCard(id);
                        }
        }

        //vid[cube].bg0rom.text(vec(1,10), str);
    }

    void onNeighborRemove(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {
		if(firstID != 0 && secondID != 0)
			return;

        //LOG("Neighbor Remove: %02x:%d - %02x:%d\n", firstID, firstSide, secondID, secondSide);

        //if (firstID < arraysize(counters)) {
        //    counters[firstID].neighborRemove++;
        //}
        //if (secondID < arraysize(counters)) {
        //    counters[secondID].neighborRemove++;
        //}

		counters[0].neighborAdd--;
    }

	void newRound()
	{
		counters[0].currentNumber = random.randint(MINNUMBER, MAXNUMBER);
		displayNumber(0);
	}

    void onNeighborAdd(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {
		if(firstID != 0 && secondID != 0)
			return;

		unsigned player;
		if(firstID == 0)
			player = secondID;
		else
			player = firstID;

        //LOG("Neighbor Add: %02x:%d - %02x:%d\n", firstID, firstSide, secondID, secondSide);
		LOG("Player %d checking in with number %d - Center has number %d\n", player, counters[player].currentNumber, counters[0].currentNumber);

		if(counters[player].currentNumber == counters[0].currentNumber) {
			LOG("Player %d just scored\n", player);
			counters[player].score++;

			newRound();

			displayScore(player);
		}
		else {
			LOG("Player %d did not score\n", player);
		}

        //if (firstID < arraysize(counters)) {
        //    counters[firstID].neighborAdd++;
        //}
        //if (secondID < arraysize(counters)) {
        //    counters[secondID].neighborAdd++;
        //}

		counters[0].neighborAdd++;
    }

    
};

void main()
{
    static NumberSmash numberSmash;

    numberSmash.setup();

    while (1){
        System::paint();
        //for (CubeID cube : CubeSet::connected())
             //   numberSmash.drawCard(cube);
        
       // numberSmash.frame++;
        //if(numberSmash.frame == 9)
        //{
         //   numberSmash.frame = 0;
       // }
        
    }
}
