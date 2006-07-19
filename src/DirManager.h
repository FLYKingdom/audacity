/**********************************************************************

  Audacity: A Digital Audio Editor

  DirManager.h

  Dominic Mazzoni

**********************************************************************/

#ifndef _DIRMANAGER_
#define _DIRMANAGER_

#include <wx/list.h>
#include <wx/string.h>
#include <wx/filename.h>
#include <wx/hashmap.h>

#include "WaveTrack.h"

class wxHashTable;
class BlockFile;
class SequenceTest;

#define FSCKstatus_CLOSEREQ 0x1
#define FSCKstatus_CHANGED  0x2

WX_DECLARE_HASH_MAP(int, int, wxIntegerHash, wxIntegerEqual, DirHash);
WX_DECLARE_HASH_MAP(wxString,BlockFile *,wxStringHash,wxStringEqual,BlockHash);

class DirManager: public XMLTagHandler {
 public:

   // MM: Construct DirManager with refcount=1
   DirManager();

   // MM: Only called by Deref() when refcount reaches zero.
   virtual ~DirManager();

   static void SetTempDir(wxString _temp) { globaltemp = _temp; }

   // MM: Ref count mechanism for the DirManager itself
   void Ref();
   void Deref();

   // Returns true on success.
   // If SetProject is told NOT to create the directory
   // but it doesn't already exist, SetProject fails and returns false.
   bool SetProject(wxString & projPath, wxString & projName, bool create);

   wxString GetProjectName();

   wxLongLong GetFreeDiskSpace();

   BlockFile *NewSimpleBlockFile(samplePtr sampleData, sampleCount sampleLen,
                                 sampleFormat format);
   BlockFile *NewAliasBlockFile( wxString aliasedFile, sampleCount aliasStart,
                                 sampleCount aliasLen, int aliasChannel);

   // Adds one to the reference count of the block file,
   // UNLESS it is "locked", then it makes a new copy of
   // the BlockFile.
   BlockFile *CopyBlockFile(BlockFile *b);

   BlockFile *LoadBlockFile(const wxChar **attrs, sampleFormat format);
   void SaveBlockFile(BlockFile *f, int depth, FILE *fp);

#if LEGACY_PROJECT_FILE_SUPPORT
   BlockFile *LoadBlockFile(wxTextFile * in, sampleFormat format);
   void SaveBlockFile(BlockFile * f, wxTextFile * out);
#endif

   bool MoveToNewProjectDirectory(BlockFile *f);
   bool CopyToNewProjectDirectory(BlockFile *f);

   bool EnsureSafeFilename(wxFileName fName);

   void Ref(BlockFile * f);
   void Deref(BlockFile * f);

   // For debugging only
   int GetRefCount(BlockFile * f);

   void SetLoadingTarget(BlockFile **target) { mLoadingTarget = target; }
   void SetLoadingFormat(sampleFormat format) { mLoadingFormat = format; }
   void SetLoadingBlockLength(sampleCount len) { mLoadingBlockLen = len; }
   bool HandleXMLTag(const wxChar *tag, const wxChar **attrs);
   XMLTagHandler *HandleXMLChild(const wxChar *tag) { return NULL; }
   void WriteXML(int depth, FILE *fp) { }
   bool AssignFile(wxFileName &filename,wxString value,bool check);

   static void CleanTempDir(bool startup);
   int ProjectFSCK(bool);
   
   wxString GetDataFilesDir() const;

 private:

   // Create new unique track name
   wxString NewTrackName();

   wxFileName MakeBlockFileName();
   wxFileName MakeBlockFilePath(wxString value);

   // Create new unique names
   wxString NewTempBlockName();
   wxString NewBlockName();

   //////////////////////////

   int mRef; // MM: Current refcount

   BlockHash blockFileHash; // repository for blockfiles
   DirHash   dirTopPool;    // available toplevel dirs
   DirHash   dirTopFull;    // full toplevel dirs
   DirHash   dirMidPool;    // available two-level dirs
   DirHash   dirMidFull;    // full two-level dirs

   void BalanceInfoDel(wxString);
   void BalanceInfoAdd(wxString);
   void BalanceFileAdd(int);
   int BalanceMidAdd(int, int);

   static bool dontDeleteTempFiles;

   wxString projName;
   wxString projPath;
   wxString projFull;

   wxString lastProject;

   wxStringList aliasList;

   BlockFile **mLoadingTarget;
   sampleFormat mLoadingFormat;
   sampleCount mLoadingBlockLen;

   static wxString globaltemp;
   wxString mytemp;
   static int numDirManagers;

   friend class SequenceTest;
};

#endif

// Indentation settings for Vim and Emacs and unique identifier for Arch, a
// version control system. Please do not modify past this point.
//
// Local Variables:
// c-basic-offset: 3
// indent-tabs-mode: nil
// End:
//
// vim: et sts=3 sw=3
// arch-tag: 5ba78795-b72e-4b1d-b408-4dc10035b0a4

