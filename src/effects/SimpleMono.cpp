/**********************************************************************

  Audacity: A Digital Audio Editor

  SimpleMono.cpp

  Dominic Mazzoni

*******************************************************************//**
\class EffectSimpleMono
\brief An abstract Effect class that simplifies the implementation of a basic
  monaural effect.  Inherit from it if your effect just modifies
  a single track in place and doesn't care how many samples
  it gets at a time.  
  
  Your derived class only needs to implement
  GetEffectName, GetEffectAction, and ProcessSimpleMono.

*//*******************************************************************/


#include "../Audacity.h"

#include "SimpleMono.h"
#include "../WaveTrack.h"

#include <math.h>

bool EffectSimpleMono::Process()
{
   //Iterate over each track
   this->CopyInputWaveTracks(); // Set up m_pOutputWaveTracks.

   TrackListIterator iterOut(m_pOutputWaveTracks);
   WaveTrack* pOutWaveTrack = (WaveTrack*)(iterOut.First());
   mCurTrackNum = 0;
   bool bGoodResult = true;
   while ((pOutWaveTrack != NULL) && bGoodResult)
   {
      //Get start and end times from track
      double trackStart = pOutWaveTrack->GetStartTime();
      double trackEnd = pOutWaveTrack->GetEndTime();

      //Set the current bounds to whichever left marker is
      //greater and whichever right marker is less:
      mCurT0 = mT0 < trackStart? trackStart: mT0;
      mCurT1 = mT1 > trackEnd? trackEnd: mT1;

      // Process only if the right marker is to the right of the left marker
      if (mCurT1 > mCurT0) {

         //Transform the marker timepoints to samples
         longSampleCount start = pOutWaveTrack->TimeToLongSamples(mCurT0);
         longSampleCount end = pOutWaveTrack->TimeToLongSamples(mCurT1);
         
         //Get the track rate and samples
         mCurRate = pOutWaveTrack->GetRate();
         mCurChannel = pOutWaveTrack->GetChannel();

         //NewTrackSimpleMono() will returns true by default
         if (!NewTrackSimpleMono())
            bGoodResult = false;

         //ProcessOne() (implemented below) processes a single track
         if (!ProcessOne(pOutWaveTrack, start, end))
            bGoodResult = false;
      }
      
      //Iterate to the next track
      pOutWaveTrack = (WaveTrack*)(iterOut.Next());
      mCurTrackNum++;
   }

   this->ReplaceProcessedWaveTracks(bGoodResult); 
   return bGoodResult;
}


//ProcessOne() takes a track, transforms it to bunch of buffer-blocks,
//and executes ProcessSimpleMono on these blocks
bool EffectSimpleMono::ProcessOne(WaveTrack * track,
                                  longSampleCount start, longSampleCount end)
{
   longSampleCount s;
   //Get the length of the buffer (as double). len is
   //used simple to calculate a progress meter, so it is easier
   //to make it a double now than it is to do it later 
   double len = (double)(end - start);

   //Initiate a processing buffer.  This buffer will (most likely)
   //be shorter than the length of the track being processed.
   float *buffer = new float[track->GetMaxBlockSize()];

   //Go through the track one buffer at a time. s counts which
   //sample the current buffer starts at.
   s = start;
   while (s < end) {
      //Get a block of samples (smaller than the size of the buffer)
      sampleCount block = track->GetBestBlockSize(s);

      //Adjust the block size if it is the final block in the track
      if (s + block > end)
         block = end - s;

      //Get the samples from the track and put them in the buffer
      track->Get((samplePtr) buffer, floatSample, s, block);

      //Process the buffer.  If it fails, clean up and exit.
      if (!ProcessSimpleMono(buffer, block)) {
         delete[]buffer;

         //Return false because the effect failed.
         return false;
      }

      //Processing succeeded. copy the newly-changed samples back 
      //onto the track.
      track->Set((samplePtr) buffer, floatSample, s, block);

      //Increment s one blockfull of samples
      s += block;

      //Update the Progress meter
      if (TrackProgress(mCurTrackNum, (s - start) / len))
         return false;
   }

   //Clean up the buffer
   delete[]buffer;

   //Return true because the effect processing succeeded.
   return true;
}

//null implementation of NewTrackSimpleMono
bool EffectSimpleMono::NewTrackSimpleMono()
{
   return true;
}

// Indentation settings for Vim and Emacs and unique identifier for Arch, a
// version control system. Please do not modify past this point.
//
// Local Variables:
// c-basic-offset: 3
// indent-tabs-mode: nil
// End:
//
// vim: et sts=3 sw=3
// arch-tag: 2ae40741-e96f-444c-9140-dcfb3455532a

