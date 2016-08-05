#include <Arduino.h>
#include "Singer.h"

	Singer::Singer()
	{
		buzzerPin = 13;
		currSongIx = 0;
		lastPlay =0 ;
		singing = false;
	}
	
	void Singer::setup(int buzzPin)
	{
		buzzerPin = buzzPin;
	}
	
	char *  Singer::strwrd(char *s, char *buf, size_t len, char *delim)
	{
		s += strspn(s, delim);
		int n = strcspn(s, delim); 
		if (len-1 < n)           
			n = len-1;
		memcpy(buf, s, n);
		buf[n] = 0;
		s += n;
		return (*s == 0) ? NULL : s;
	}

	
	
	String Singer::createSong(int minValue,int maxValue)
	{
	  int currNote= 0;
	  int noteIx;
	  int durationSign=1;
	  int noteSign = 1;
	  String noteIxStr ="";
	  noteIxStr.reserve(30);
	  String durationStr="";
	  durationStr.reserve(30);
	  int startIx ='a';
	  minValue = minValue<0?0:minValue;
	  maxValue = maxValue> maxSafeFeeling?maxSafeFeeling-1:maxValue;
	  for(int i=0;i<songLen;i++)
	  {
		if( currNote ==i)
		{
		  currNote = i+( rand()%3)+1;
		  noteIx =  minValue + (rand() % maxValue); //keep song in lower bars if we had danger recently    
		  noteSign = (noteIx %2 ==0)?1:-1;
		}
		noteIx += noteSign;
		noteIx = noteIx<0|| noteIx>(maxSafeFeeling -1)? ((minValue+maxValue)/2) :noteIx; //reset if we went off the note array
		noteIxBar[i] = noteIx;		
		int val = startIx+noteIx;
		noteIxStr += char(val);

		int duration = ( rand() +millis()) % 4+1;
		durationBar[i] = duration;
		durationStr += String(duration);
	  }
	
	  String fullSong = String(noteIxStr)+sep+durationStr;
	  fullSong.reserve(30);
	  
	  return fullSong.c_str();
	}

	void Singer::playSong(char* newSong)
	{
			//load the song
		char * song = new char[2*songLen* sizeof(char)];
		char * pauses = new char[2*songLen* sizeof(char)];
		newSong=strwrd ( newSong,song,2*songLen* sizeof(char), const_cast<char*>(sep));
		strwrd ( newSong,pauses,2*songLen* sizeof(char), const_cast<char*>(sep));

		int i=0;
		int charIx ='a';
		int zeroIx ='0';
		while(i< songLen)
		{
			int noteIx = (char)song[i] ;
			noteIx = noteIx - charIx;
			int pause = (char) pauses[i];
			int pauseMillis = pause - zeroIx;
			noteIxBar[i] = noteIx;
			durationBar[i] = pauseMillis*200;
			i++;
		}
		delete song;
		delete pauses;
		currSongIx =i<2?songLen:0;
		singing = currSongIx ==0;
	}

	
	void Singer::stop()
	{
		noTone(buzzerPin);
		currSongIx= songLen+1;
		singing = false;
	}
	
void Singer::update(long currTime)
	{
	   if(  (currTime - lastPlay)>durationBar[currSongIx])
	  {
			if(currSongIx<songLen)
			{
			 tone(buzzerPin,notes[noteIxBar[currSongIx]],durationBar[currSongIx]);
			 lastPlay = currTime;
			 currSongIx++;
			 }else{	
				noTone(buzzerPin);
				singing=false;
			 }

	  }
	  
	  
	}
	

	int Singer::getCurrSongIx()
	{
		return currSongIx;
	}