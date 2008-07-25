/**********************************************************************

  Audacity: A Digital Audio Editor

  ODWaveTrackTaskQueue.h

  Created by Michael Chinen (mchinen) on 6/11/08
  Audacity(R) is copyright (c) 1999-2008 Audacity Team.
  License: GPL v2.  See License.txt.

******************************************************************//**

\class ODWaveTrackTaskQueue
\brief watches over all to be done (not yet started and started but not finished)
tasks associated with a WaveTrack.

*//*******************************************************************/

#include "ODWaveTrackTaskQueue.h"
#include "ODTask.h"
#include "ODManager.h"
/// Constructs an ODWaveTrackTaskQueue
ODWaveTrackTaskQueue::ODWaveTrackTaskQueue()
{
}
   
ODWaveTrackTaskQueue::~ODWaveTrackTaskQueue()
{
   //we need to delete all ODTasks.  We will have to block or wait until block for the active ones.
   for(unsigned int i=0;i<mTasks.size();i++)
   {
      mTasks[i]->TerminateAndBlock();//blocks if active.
      //small chance we may have rea-added the task back into the queue from a diff thread.  - so remove it if we have.
      ODManager::Instance()->RemoveTaskIfInQueue(mTasks[i]);
      delete mTasks[i];
   }
   
}

///returns true if the argument is in the WaveTrack list.
bool ODWaveTrackTaskQueue::ContainsWaveTrack(WaveTrack* track)
{
   mTracksMutex.Lock();
   for(unsigned int i=0;i<mTracks.size();i++)
   {
      if(mTracks[i]==track)
      {
         mTracksMutex.Unlock();
         return true;
      }
   }
   mTracksMutex.Unlock();
   return false;
}   
///Adds a track to the associated list.
void ODWaveTrackTaskQueue::AddWaveTrack(WaveTrack* track)
{
   
   mTracksMutex.Lock();
   if(track)
      mTracks.push_back(track);
   
   mTracksMutex.Unlock();
}

void ODWaveTrackTaskQueue::AddTask(ODTask* task)
{
   mTasksMutex.Lock();
   mTasks.push_back(task);
   mTasksMutex.Unlock();
   
   //take all of the tracks in the task. 
   mTracksMutex.Lock();
   for(int i=0;i<task->GetNumWaveTracks();i++)
   {
      //ODTask::GetWaveTrack is not threadsafe, but I think we are safe anyway, because the task isn't being run yet?
      mTracks.push_back(task->GetWaveTrack(i));
   } 
    
   mTracksMutex.Unlock();

}

///Removes a track from the list.  Also notifies mTasks to stop using references
///to the instance in a thread-safe manner (may block)
void ODWaveTrackTaskQueue::RemoveWaveTrack(WaveTrack* track)
{
   if(track)
   {
      
      mTracksMutex.Lock();
      for(unsigned int i=0;i<mTasks.size();i++)
      {
         mTasks[i]->StopUsingWaveTrack(track);
      }
      
      mTracksMutex.Unlock();
   }
}

///changes the tasks associated with this Waveform to process the task from a different point in the track
///@param track the track to update
///@param seconds the point in the track from which the tasks associated with track should begin processing from.
void ODWaveTrackTaskQueue::DemandTrackUpdate(WaveTrack* track, double seconds)
{
   if(track)
   {
      mTracksMutex.Lock();
      for(unsigned int i=0;i<mTasks.size();i++)
      {
         mTasks[i]->DemandTrackUpdate(track,seconds);
      }
      
      mTracksMutex.Unlock();
   }
}


//Replaces all instances of a wavetracck with a new one (effectively transferes the task.)
void ODWaveTrackTaskQueue::ReplaceWaveTrack(WaveTrack* oldTrack, WaveTrack* newTrack)
{
   if(oldTrack)
   {
      mTracksMutex.Lock();
      for(unsigned int i=0;i<mTasks.size();i++)
      {
         mTasks[i]->ReplaceWaveTrack(oldTrack,newTrack);
      }
      
      mTracksMutex.Unlock();
   }
}

//returns the wavetrack at position x.
WaveTrack* ODWaveTrackTaskQueue::GetWaveTrack(size_t x)
{
   WaveTrack* ret = NULL;
   mTracksMutex.Lock();
   if(x>=0&&x<mTracks.size())
      ret = mTracks[x];
   mTracksMutex.Unlock();
   return ret;
}

///returns the number of wavetracks in this queue.
int ODWaveTrackTaskQueue::GetNumWaveTracks()
{

   int ret = 0;
   mTracksMutex.Lock();
   ret=mTracks.size();
   mTracksMutex.Unlock();
   return ret;
}

//returns true if either tracks or tasks are empty
bool ODWaveTrackTaskQueue::IsEmpty()
{
   bool isEmpty;
   mTracksMutex.Lock();
   isEmpty = mTracks.size()<=0;
   mTracksMutex.Unlock();
   
   mTasksMutex.Lock();
   isEmpty = isEmpty || mTasks.size()<=0; 
   mTasksMutex.Unlock();
   
   return isEmpty;
}
   
//returns true if the foremost task exists and is empty.
bool ODWaveTrackTaskQueue::IsFrontTaskComplete()
{
   mTasksMutex.Lock();
   if(mTasks.size())
   {
      mTasksMutex.Unlock();
      return mTasks[0]->IsComplete();
   }
   mTasksMutex.Unlock();
   return false;
}
   
///Removes and deletes the front task from the list.
void ODWaveTrackTaskQueue::RemoveFrontTask()
{
   mTasksMutex.Lock();
   if(mTasks.size())
      mTasks.erase(mTasks.begin());
   mTasksMutex.Unlock();
}
   
///Schedules the front task for immediate execution
ODTask* ODWaveTrackTaskQueue::GetFrontTask()
{
   mTasksMutex.Lock();
   if(mTasks.size())
   {
      mTasksMutex.Unlock();
      return mTasks[0];
   }
   mTasksMutex.Unlock();
   return NULL;
}
