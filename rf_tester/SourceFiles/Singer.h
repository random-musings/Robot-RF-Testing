

class Singer
{

	public:
		int buzzerPin;
		Singer();
		
		String createSong(int minValue,int maxValue);
		void playSong(char* song);
		void setup(int buzzPin);
		void stop();
		void update(long currTime);
		const int songLen = 11;
		int getCurrSongIx();
		bool singing;
		const int maxSafeFeeling = 25;
		protected:
		const char* delim=",";
		const char* sep=":";
	
	protected:
		long lastPlay;
		char *  strwrd(char *s, char *buf, size_t len, char *delim);
		int noteIxBar[30];
		int durationBar[30];		
		int currSongIx;

		// These values can also be found as constants in the Tone library (Tone.h)
/*		//WE ONLY HAV 26 SLOTS S WE WILL PAIR THIS DOWN
		int notes[50] = { 0,
		262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
		523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,
		1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
		2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951
		};
		*/

		int notes[25] = { 0,
		262,  294,  330,  370,  415,  466, 
		523,  587,  659,  740,  831,  932, 
		1047,  1175,  1319,  1480,  1661,  1865, 
		2093,  2349,  2637,  2960,  3322,  3729
		};

};
