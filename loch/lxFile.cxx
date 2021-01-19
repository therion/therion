#include "lxFile.h"

// Standard libraries
#ifndef LXDEPCHECK
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <locale.h>
#include <map>
#include <list>
#include <fstream>

#endif  
//LXDEPCHECK - standard libraries

#include "lxMath.h"
#include "img.h"

#ifdef THMSVC
#define strcasecmp _stricmp
#endif


lxFileSizeT lxFileSize::Save(lxFileBuff & ptr)
{
  lxFileSizeT s(sizeof(uint32_t));
  std::memcpy(ptr, &m_size, s);
  lxFile::switchEndian(ptr, s);
  ptr += s;
  return s;
}


lxFileSizeT lxFileSize::Load(lxFileBuff & ptr)
{
  lxFileSizeT s(sizeof(lxFileSizeT));
  std::memcpy(&m_size, ptr, s);
  lxFile::switchEndian((char *)(&this->m_size), s);
  ptr += s;
  return s;
}


lxFileSizeT lxFileDbl::Save(lxFileBuff & ptr)
{
  lxFileSizeT s(sizeof(this->m_num));
  std::memcpy(ptr, &m_num, s);
  lxFile::switchEndian(ptr, s);
  ptr += s;
  return s;
}


lxFileSizeT lxFileDbl::Load(lxFileBuff & ptr)
{
  lxFileSizeT s(sizeof(this->m_num));
  std::memcpy(&m_num, ptr, s);
  lxFile::switchEndian((char *)(&this->m_num), s);
  ptr += s;
  return s;
}



lxFileDataPtr::lxFileDataPtr()
{
  this->Clear();
}

void lxFileDataPtr::Clear()
{
  this->m_position = 0;
  this->m_size = 0;
}


lxFileSizeT lxFileDataPtr::Save(lxFileBuff & ptr)
{
  lxFileSizeT s(0);
  s += this->m_position.Save(ptr);
  s += this->m_size.Save(ptr);
  return s;
}


lxFileSizeT lxFileDataPtr::Load(lxFileBuff & ptr)
{
  lxFileSizeT s(0);
  s += this->m_position.Load(ptr);
  s += this->m_size.Load(ptr);
  return s;
}


lxFileData::lxFileData()
{
  this->m_data = NULL;
  this->m_size = 0;
  this->m_buffSize = 0;
}


void lxFileData::Clear()
{
  if (this->m_data != NULL)
    free(this->m_data);
  this->m_data = NULL;
  this->m_size = 0;
  this->m_buffSize = 0;
}

void lxFileData::Copy(lxFileSizeT size, const void * src)
{
  this->Clear();
  this->m_data = malloc(size);
  this->m_size = size;
  this->m_buffSize = size;
  memcpy(this->m_data, src, size);
}


const void * lxFileData::GetData(lxFileDataPtr ptr)
{
  if (ptr.m_size <= 1)
    return NULL;
  else
    return (void *)(&(((char *)this->m_data)[ptr.m_position]));
}

FILE * lxFileData::GetTmpFile(lxFileDataPtr ptr)
{
  FILE * rf;
  if (ptr.m_size <= 1)
    return NULL;
  rf = tmpfile();
  //rf = fopen("TMPFILE.JPG","wb");
  if (rf != NULL) {
    fwrite(&(((char *)this->m_data)[ptr.m_position]), 1, ptr.m_size, rf);
    fseek(rf, 0, SEEK_SET);
    //fclose(rf);
    //rf = fopen("TMPFILE.JPG","rb");
  }
  return rf;
}

const char * lxFileData::GetString(lxFileDataPtr ptr)
{
  if (ptr.m_size <= 1)
    return "";
  else
    return &(((char *)this->m_data)[ptr.m_position]);
}

void lxFileData::BuffResize(lxFileSizeT size)
{
  lxFileSizeT nsize;
  void * ndata;

  if (this->m_buffSize > 0)
    nsize = this->m_buffSize;
  else
    nsize = 1024;
  while (size >= nsize) nsize *= 2;
  this->m_buffSize = nsize;
  ndata = malloc(nsize);
  if (this->m_size > 0)
    memcpy(ndata, this->m_data, this->m_size);
  free(this->m_data);
  this->m_data = ndata;
}


lxFileDataPtr lxFileData::AppendStr(const char * str)
{
  lxFileDataPtr res;
  lxFileSizeT strln;
  if (str == NULL)
    return res;
  strln = strlen(str);
  if (strln == 0)
    return res;
  return this->AppendData(str, strln + 1);
}


lxFileDataPtr lxFileData::AppendFile(const char * fnm)
{
  lxFileDataPtr res;
  FILE * xf;
  xf = fopen(fnm, "rb");
  if (xf != NULL) {
    fseek(xf, 0, SEEK_END);
    lxFileSizeT fsz = ftell(xf);
    fseek(xf, 0, SEEK_SET);
    if (fsz > 0) {
      char * cdata = new char [fsz];
      lxassert(fread((void *) cdata, 1, fsz, xf) == fsz);
      res = this->AppendData(cdata, fsz);
      delete [] cdata;
    }
    fclose(xf);
  }
  return res;
}



lxFileDataPtr lxFileData::AppendData(const void * data, lxFileSizeT size)
{
  lxFileDataPtr res;
  if ((data == NULL) || (size == 0))
    return res;
  if (this->m_buffSize < this->m_size + size)
    this->BuffResize(this->m_size + size);
  res.m_position = this->m_size;
  res.m_size = size;
  memcpy(&(((char *)this->m_data)[this->m_size]), data, size);
  this->m_size += size;
  return res;
}


lxFileStation::lxFileStation()
{
  this->m_flags = 0;
}


bool lxFileIsBigEndian() {
  unsigned long i = 1;
  const unsigned char * p = (const unsigned char *) &i;
  return (p[0] != 1);
}


bool lxFile::m_bigEndian(lxFileIsBigEndian());


void lxFile::switchEndian(char * data, lxFileSizeT size)
{
  if (m_bigEndian) {
    char temp;
    lxFileSizeT index, rindex, total;
    total = size / 2;
    for (index = 0; index < total; index++) {
      rindex = size - index - 1;
      temp = data[index];
      data[index] = data[rindex];
      data[rindex] = temp;
    }
  }
}


lxFileShot::lxFileShot()
{
  this->m_flags = 0;
  this->m_threshold = 60.0;
  this->m_sectionType = LXFILE_SHOT_SECTION_NONE;
}

lxFile::lxFile()
{
  this->Clear();
}

lxFile::~lxFile()
{
  this->Clear();
}


void lxFile::Clear()
{

  this->m_surveys.clear();
  this->m_surveysData.Clear();
  this->m_nSurveys = 0;

  this->m_stations.clear();
  this->m_stationsData.Clear();
  this->m_nStations = 0;

  this->m_shots.clear();
  this->m_shotsData.Clear();

  this->m_scraps.clear();
  this->m_scrapsData.Clear();

  this->m_surfaces.clear();
  this->m_surfacesData.Clear();

  this->m_surfaceBitmaps.clear();
  this->m_surfaceBitmapsData.Clear();

}


enum {
  LXFILE_CHUNK_SURVEY = 1,
  LXFILE_CHUNK_STATION = 2,
  LXFILE_CHUNK_SHOT = 3,
  LXFILE_CHUNK_SCRAP = 4,
  LXFILE_CHUNK_SURFACE = 5,
  LXFILE_CHUNK_SURFACEBMP = 6,
};


struct lxFileChunkHdr {
  lxFileSize m_type, m_recSize, m_recCount, m_dataSize;
  lxFileSizeT Save(lxFileBuff & ptr);
  lxFileSizeT Load(lxFileBuff & ptr);
};


lxFileSizeT lxFileChunkHdr::Save(lxFileBuff & ptr)
{
  lxFileSizeT s(0);
  s += this->m_type.Save(ptr);
  s += this->m_recSize.Save(ptr);
  s += this->m_recCount.Save(ptr);
  s += this->m_dataSize.Save(ptr);
  return s;
}



lxFileSizeT lxFileChunkHdr::Load(lxFileBuff & ptr)
{
  lxFileSizeT s(0);
  s += this->m_type.Load(ptr);
  s += this->m_recSize.Load(ptr);
  s += this->m_recCount.Load(ptr);
  s += this->m_dataSize.Load(ptr);
  return s;
}




lxFileSizeT lxFile3Point::Save(lxFileBuff & ptr)
{
  lxFileSizeT s(0);
  s += this->m_c[0].Save(ptr);
  s += this->m_c[1].Save(ptr);
  s += this->m_c[2].Save(ptr);
  return s;
}


lxFileSizeT lxFile3Angle::Load(lxFileBuff & ptr)
{
  lxFileSizeT s(0);
  s += this->m_v[0].Load(ptr);
  s += this->m_v[1].Load(ptr);
  s += this->m_v[2].Load(ptr);
  return s;
}


void lxFile::ExportLOX(const char * fn)
{
  
  this->m_error.clear();

  this->m_file = fopen(fn,"wb");
  if (this->m_file == NULL) {
    this->m_error = "unable to open file for output";
    return;
  }
  
  // write data
  lxFileChunkHdr chunkHdr;
  char * tmpPtr, * chunkHdrPtr, chunkHdrBuffer [sizeof(chunkHdr)];
  lxFileSizeT chunkHdrSize, tmpSize;
  bool writeErr;

  writeErr = false;
  lxFileSizeT x, size;

#define lxFileExportItem(expID, expClass, expRecs, expData) \
  size = this->expRecs.size(); \
  if ((!writeErr) && (size > 0)) { \
    char * tmpData = new char [size * sizeof(expClass)]; \
    tmpPtr = tmpData; tmpSize = 0; \
    expClass##_list::iterator tmpIter = this->expRecs.begin(); \
    for (x = 0; x < size; x++) { \
      tmpSize += tmpIter->Save(tmpPtr); \
      tmpIter++; \
    } \
    chunkHdr.m_type = expID; \
    chunkHdr.m_recSize = tmpSize; \
    chunkHdr.m_recCount = size; \
    chunkHdr.m_dataSize = this->expData.m_size; \
    chunkHdrPtr = chunkHdrBuffer; \
    chunkHdrSize = chunkHdr.Save(chunkHdrPtr); \
    if (fwrite(&chunkHdrBuffer, chunkHdrSize, 1, this->m_file) != 1) \
      writeErr = true; \
    if (fwrite(tmpData, tmpSize, 1, this->m_file) != 1) \
      writeErr = true; \
    if (chunkHdr.m_dataSize > 0) \
      if (fwrite(this->expData.m_data, chunkHdr.m_dataSize, 1, this->m_file) != 1) \
        writeErr = true; \
    delete [] tmpData; \
  }

  lxFileExportItem(LXFILE_CHUNK_SURVEY, lxFileSurvey, m_surveys, m_surveysData);
  lxFileExportItem(LXFILE_CHUNK_STATION, lxFileStation, m_stations, m_stationsData);
  lxFileExportItem(LXFILE_CHUNK_SHOT, lxFileShot, m_shots, m_shotsData);
  lxFileExportItem(LXFILE_CHUNK_SCRAP, lxFileScrap, m_scraps, m_scrapsData);
  lxFileExportItem(LXFILE_CHUNK_SURFACE, lxFileSurface, m_surfaces, m_surfacesData);
  lxFileExportItem(LXFILE_CHUNK_SURFACEBMP, lxFileSurfaceBitmap, m_surfaceBitmaps, m_surfaceBitmapsData);

  /*
  size = this->m_surveys.size();
  if ((!writeErr) && (size > 0)) {
    
    lxFileSurvey * tmpData = new lxFileSurvey [size];
    lxFileSurvey_list::iterator tmpIter = this->m_surveys.begin();
    for (x = 0; x < size; x++) {
      tmpData[x] = *tmpIter;
      tmpIter++;
    }
    
    chunkHdr.m_type = LXFILE_CHUNK_SURVEY;
    chunkHdr.m_recSize = sizeof(lxFileSurvey);
    chunkHdr.m_recCount = size;
    chunkHdr.m_dataSize = this->m_surveysData.m_size;

    if (fwrite(&chunkHdr, sizeof(chunkHdr), 1, this->m_file) != 1)
      writeErr = true;

    if (fwrite(tmpData, chunkHdr.m_recSize, size, this->m_file) != size)
      writeErr = true;

    if (chunkHdr.m_dataSize > 0)
      if (fwrite(this->m_surveysData.m_data, chunkHdr.m_dataSize, 1, this->m_file) != 1)
        writeErr = true;

    delete [] tmpData;
  }
  */

  fclose(this->m_file);
  if (writeErr)
    this->m_error = "error writing to file.";

}


void lxFile::ImportLOX(const char * fn)
{
  this->m_error.clear();

  this->m_file = fopen(fn,"rb");
  if (this->m_file == NULL) {
    this->m_error = "unable to open file for input";
    return;
  }

  lxFileChunkHdr chunkHdr;
  lxFileSizeT i, orig_size, orig_survey_id, orig_station_id;
  char * tmpPtr, * chunkHdrPtr, * tmpRecsData, chunkHdrBuffer [sizeof(chunkHdr)];
  lxFileSizeT chunkHdrSize;

  chunkHdrPtr = chunkHdrBuffer;
  chunkHdrSize = chunkHdr.Save(chunkHdrPtr);

  orig_survey_id = 0;
  for(lxFileSurvey_list::iterator itsurv = this->m_surveys.begin(); itsurv != this->m_surveys.end(); itsurv++) {
    if (orig_survey_id < itsurv->m_id)
      orig_survey_id = itsurv->m_id;
  }

  orig_station_id = 0;
  for(lxFileStation_list::iterator itstat = this->m_stations.begin(); itstat != this->m_stations.end(); itstat++) {
    if (orig_station_id < itstat->m_id)
      orig_station_id = itstat->m_id;
  }

  bool readErr = false;
  while ((!readErr) && (!feof(this->m_file)) && (fread(chunkHdrBuffer, chunkHdrSize, 1, this->m_file) == 1)) {
  
    chunkHdrPtr = chunkHdrBuffer;
    chunkHdr.Load(chunkHdrPtr);

    switch (chunkHdr.m_type) {

      /*
      case LXFILE_CHUNK_SURVEY:
        if (chunkHdr.m_recCount > 0) {
          orig_size = this->m_surveysData.m_size;
          lxFileSurvey * tmpRecs = new lxFileSurvey [chunkHdr.m_recCount];
          if   (fread(tmpRecs, sizeof(lxFileSurvey), chunkHdr.m_recCount, this->m_file) != chunkHdr.m_recCount)
            readErr = true;
          if ((!readErr) && (chunkHdr.m_dataSize > 0)) {
            void * tmpData = malloc(chunkHdr.m_dataSize);
            if (fread(tmpData, chunkHdr.m_dataSize, 1, this->m_file) != 1)
              readErr = true;
            else
              this->m_surveysData.AppendData(tmpData, chunkHdr.m_dataSize);
            free(tmpData);
          }
          if (!readErr) {
            for(i = 0; i < chunkHdr.m_recCount; i++) {
              tmpRecs[i].m_namePtr.m_position += orig_size;
              tmpRecs[i].m_titlePtr.m_position += orig_size;
              this->m_surveys.push_back(tmpRecs[i]);
            }
          }
          delete [] tmpRecs;
        }
        break; 
      */

#define lxFileStartImportItem(impID, impClass, impData) \
      case impID: \
        if (chunkHdr.m_recCount > 0) { \
          orig_size = this->impData.m_size; \
          tmpRecsData = new char [chunkHdr.m_recSize]; \
          impClass tmpRec; \
          if (fread(tmpRecsData, chunkHdr.m_recSize, 1, this->m_file) != 1) \
            readErr = true; \
          if ((!readErr) && (chunkHdr.m_dataSize > 0)) { \
            void * tmpData = malloc(chunkHdr.m_dataSize); \
            if (fread(tmpData, chunkHdr.m_dataSize, 1, this->m_file) != 1) \
              readErr = true; \
            else \
              this->impData.AppendData(tmpData, chunkHdr.m_dataSize); \
            free(tmpData); \
          } \
          if (!readErr) { \
            tmpPtr = tmpRecsData; \
            for(i = 0; i < chunkHdr.m_recCount; i++) {  \
              tmpRec.Load(tmpPtr);

#define lxFileEndImportItem() \
            } \
          } \
          delete [] tmpRecsData; \
        } \
        break; 

      lxFileStartImportItem(LXFILE_CHUNK_SURVEY, lxFileSurvey, m_surveysData)
        tmpRec.m_id += orig_survey_id;
        tmpRec.m_namePtr.m_position += orig_size;
        tmpRec.m_titlePtr.m_position += orig_size;
        this->m_surveys.push_back(tmpRec);
        this->m_nSurveys++;
      lxFileEndImportItem()

      lxFileStartImportItem(LXFILE_CHUNK_STATION, lxFileStation, m_stationsData)
        tmpRec.m_id += orig_station_id;
        tmpRec.m_surveyId += orig_survey_id;
        tmpRec.m_namePtr.m_position += orig_size;
        tmpRec.m_commentPtr.m_position += orig_size;
        this->m_stations.push_back(tmpRec);
        this->m_nStations++;
      lxFileEndImportItem()

      lxFileStartImportItem(LXFILE_CHUNK_SHOT, lxFileShot, m_shotsData)
        tmpRec.m_from += orig_station_id;
        tmpRec.m_to += orig_station_id;
        tmpRec.m_surveyId += orig_survey_id;
        this->m_shots.push_back(tmpRec);
      lxFileEndImportItem()

      lxFileStartImportItem(LXFILE_CHUNK_SCRAP, lxFileScrap, m_scrapsData)
        tmpRec.m_pointsPtr.m_position += orig_size;
        tmpRec.m_3AnglesPtr.m_position += orig_size;
        tmpRec.m_surveyId += orig_survey_id;
        this->m_scraps.push_back(tmpRec);
      lxFileEndImportItem()

      lxFileStartImportItem(LXFILE_CHUNK_SURFACE, lxFileSurface, m_surfacesData)
        tmpRec.m_dataPtr.m_position += orig_size;
        this->m_surfaces.push_back(tmpRec);
      lxFileEndImportItem()

      lxFileStartImportItem(LXFILE_CHUNK_SURFACEBMP, lxFileSurfaceBitmap, m_surfaceBitmapsData)
        tmpRec.m_dataPtr.m_position += orig_size;
        this->m_surfaceBitmaps.push_back(tmpRec);
      lxFileEndImportItem()

      default:
        readErr = true;
    }
  }

  fclose(this->m_file);
}


lxFileSizeT lxFileSplitTokens(std::string& str, char ** tokens, lxFileSizeT max_tokens)
{
  lxFileSizeT nt = 0, sp = 0;
  bool inside = false;
  if (max_tokens == 0)
    return 0;
  max_tokens--;
  for (auto cc = str.begin(); (sp < str.size()) && (nt < max_tokens); sp++, cc++) {
    if (inside && (*cc < 33)) {
      *cc = 0;
      inside = false;
      nt++;
    } else if ((!inside) && (*cc > 32)) {
      inside = true;
      tokens[nt] = &*cc;
    }
  }
  return nt;
}

bool lxFile__CheckLRUD(double & du, double & dd, double & dl, double & dr, double mv, double mh) {
  if ((du <= 0.0) && (dd <= 0.0) && (dl <= 0.0) && (dr <= 0.0)) {
    return false;
  } else {
    if (mv > 0.0) {
      if (du < 0.0) du = mv;
      if (dd < 0.0) dd = mv;
      if (dl < 0.0) dl = mv;
      if (dr < 0.0) dr = mv;
      return true;
    } else {
      // LR
      if ((du < 0.0) && (dd >= 0.0)) 
        du = dd;
      if ((dd < 0.0) && (du >= 0.0)) 
        dd = du;
      // UD
      if ((dl < 0.0) && (dr >= 0.0)) 
        dl = dr;
      if ((dr < 0.0) && (dl >= 0.0)) {
        dr = dl;
        if (dr > mh) dr = mh;
      }
      
      if (du < 0.0) {
        du = dd = (dl + dr) / 2.0;
      }
      if (dl < 0.0) {
        dl = dr = (du + dd) / 2.0;
        if (dr > mh) dr = mh;
      }
      return true;
    }
  }
}


lxFileSizeT lxFileSurvey::Save(lxFileBuff & ptr)
{
  lxFileSizeT s(0);
  s += this->m_id.Save(ptr);
  s += this->m_namePtr.Save(ptr);
  s += this->m_parent.Save(ptr);
  s += this->m_titlePtr.Save(ptr);
  return s;
}


lxFileSizeT lxFileSurvey::Load(lxFileBuff & ptr)
{
  lxFileSizeT s(0);
  s += this->m_id.Load(ptr);
  s += this->m_namePtr.Load(ptr);
  s += this->m_parent.Load(ptr);
  s += this->m_titlePtr.Load(ptr);
  return s;
}


lxFileSurvey * lxFile::NewSurvey()
{
  lxFileSurvey tmp;
  tmp.m_id = this->m_nSurveys++;
  this->m_surveys.push_back(tmp);
  return &(this->m_surveys.back());
}


lxFileStation * lxFile::NewStation()
{
  lxFileStation tmp;
  tmp.m_id = this->m_nStations++;
  this->m_stations.push_back(tmp);
  return &(this->m_stations.back());
}


lxFileShot * lxFile::NewShot()
{
  this->m_shots.push_back(lxFileShot());
  return &(this->m_shots.back());
}


#define PltLrudNaN(x) {if ((x < 0.0) || (long(x*100) == 99900)) x = -999.0;}


void lxFile::ImportPLT(const char * fn)
{
  this->m_error.clear();
  char* prevlocale_ptr = setlocale(LC_NUMERIC,NULL);
  const std::string prevlocale = prevlocale_ptr ? prevlocale_ptr : "";
  setlocale(LC_NUMERIC,"C");

  bool lrudOK, lrudOKPrev = false;
  char * tok[16];
  lxFileDbl lrud[4], lrudPrev[4];
  lrudPrev[0] = lrudPrev[1] = lrudPrev[2] = lrudPrev[3] = -1.0;
  lxFileSizeT nt;

  std::ifstream input(fn);
  if (!input.is_open()) {
    this->m_error = "unable to open file for input";
    return;
  }

#define tok2num(v, n) v = atof((const char *)tok[n]);

  lxFileSurvey * tmpSurvey;
  lxFileShot * shPtr;
  lxFileStation * stPtr, * stPtrPrev;
  stPtrPrev = 0;
  bool hasPT = false;

  char * sname = new char [strlen(fn)+1];
  int x, xx;
  xx = 0;
  for (x = (strlen(fn) - 1); x >= 0; x--) {
    if ((fn[x] == '\\') || (fn[x] == '/')) {
      xx = x + 1;
      break;
    }
  }
  for (x = xx; (x < int(strlen(fn))) && (strcasecmp(&(fn[x]),".PLT") != 0); x++) {
    sname[x - xx] = fn[x];
    sname[x - xx + 1] = 0;
  }
  tmpSurvey = this->NewSurvey();
  tmpSurvey->m_namePtr = this->m_surveysData.AppendStr(sname);
  delete [] sname;

  std::string line;
  while (!input.eof()) {
    lxassert(std::getline(input, line));
    nt = lxFileSplitTokens(line, &(tok[0]), 16);
    (void)nt;
    switch (*(tok[0])) {
      case 'M':
      case 'D':
        if (!hasPT && (*(tok[0]) == 'D')) {
          *(tok[0]) = 'M';
        }
        tok2num(lrud[0],6);PltLrudNaN(lrud[0]);
        tok2num(lrud[1],9);PltLrudNaN(lrud[1]);
        tok2num(lrud[2],7);PltLrudNaN(lrud[2]);
        tok2num(lrud[3],8);PltLrudNaN(lrud[3]);
        lrudOK = lxFile__CheckLRUD(lrud[0], lrud[1], lrud[2], lrud[3], 2.0, 5.0);
        if (lrudOK) {
          lrud[0] *= 0.3048;
          lrud[1] *= 0.3048;
          lrud[2] *= 0.3048;
          lrud[3] *= 0.3048;
        }
        stPtr = this->NewStation();
        tok2num(stPtr->m_c[0],2);
        tok2num(stPtr->m_c[1],1);
        tok2num(stPtr->m_c[2],3);
        stPtr->m_c[0] *= 0.3048;
        stPtr->m_c[1] *= 0.3048;
        stPtr->m_c[2] *= 0.3048;
        stPtr->m_surveyId = tmpSurvey->m_id;
        hasPT = true;
        if (*(tok[0]) == 'D') {

          shPtr = this->NewShot();
          shPtr->m_from = stPtrPrev->m_id;
          shPtr->m_to = stPtr->m_id;
          shPtr->m_fLRUD[0] = lrudPrev[0];
          shPtr->m_fLRUD[1] = lrudPrev[1];
          shPtr->m_fLRUD[2] = lrudPrev[2];
          shPtr->m_fLRUD[3] = lrudPrev[3];
          shPtr->m_tLRUD[0] = lrud[0];
          shPtr->m_tLRUD[1] = lrud[1];
          shPtr->m_tLRUD[2] = lrud[2];
          shPtr->m_tLRUD[3] = lrud[3];
          shPtr->m_surveyId = tmpSurvey->m_id;

          if (lrudOK && lrudOKPrev) {
            shPtr->m_sectionType = LXFILE_SHOT_SECTION_OVAL;
          } else {
            shPtr->m_sectionType = LXFILE_SHOT_SECTION_NONE;
          }

        }
        lrudPrev[0] = lrud[0];
        lrudPrev[1] = lrud[1];
        lrudPrev[2] = lrud[2];
        lrudPrev[3] = lrud[3];
        lrudOKPrev = lrudOK;
        stPtrPrev = stPtr;
        break;
    }
  }

  setlocale(LC_NUMERIC,prevlocale.c_str());
}

struct imp3Dpos {
  double x, y, z;
  imp3Dpos(double xx, double yy, double zz) : x(xx), y(yy), z(zz) {}
};

bool operator < (const imp3Dpos & p1,
    const imp3Dpos & p2)
{
  if (p1.x < p2.x)
    return true;
  if (p1.x > p2.x)
    return false;
  if (p1.y < p2.y)
    return true;
  if (p1.y > p2.y)
    return false;
  if (p1.z < p2.z)
    return true;
  return false;
}


void lxFile::Import3D(const char * fn)
{

  this->m_error.clear();
  img_point imgpt;
  img * pimg;
  int result;

  bool lrudOK, lrudOKPrev = false;
  lxFileDbl lrud[4], lrudPrev[4];
  lrudPrev[0] = lrudPrev[1] = lrudPrev[2] = lrudPrev[3] = -1.0;

  pimg = img_open(fn);
  if (pimg == NULL) {
    this->m_error = "unable to open file for input";
    return;
  }

  lxFileSurvey * tmpSurvey;
  lxFileShot * shPtr;
  lxFileStation * stPtr, * stPtrPrev, * stPtrLRUD, * stPtrLRUDPrev;
  stPtr = NULL;
  stPtrPrev = NULL;
  stPtrLRUDPrev = NULL;
  bool hasPT = false;

  char * sname = new char [strlen(fn)+1];
  int x, xx;
  xx = 0;
  for (x = (strlen(fn) - 1); x >= 0; x--) {
    if ((fn[x] == '\\') || (fn[x] == '/')) {
      xx = x + 1;
      break;
    }
  }
  for (x = xx; (x < int(strlen(fn))) && (strcasecmp(&(fn[x]),".PLT") != 0); x++) {
    sname[x - xx] = fn[x];
    sname[x - xx + 1] = 0;
  }
  tmpSurvey = this->NewSurvey();
  tmpSurvey->m_namePtr = this->m_surveysData.AppendStr(sname);
  delete [] sname;

  std::map<std::string, lxFileStation*> label_map;
  std::map<std::string, lxFileStation*>::iterator lmi;
  std::map<imp3Dpos, lxFileStation*> pos_map;
  std::map<imp3Dpos, lxFileStation*>::iterator pmi;

  // Create all stations with names
  // Set HAS_WALLS flag if applicable
  do {    
    result = img_read_item(pimg, &imgpt);
    switch (result) {
      case img_LABEL:
        if (pimg->label != NULL) {
          stPtr = this->NewStation();
          stPtr->m_c[0] = imgpt.x;
          stPtr->m_c[1] = imgpt.y;
          stPtr->m_c[2] = imgpt.z;
          stPtr->m_surveyId = tmpSurvey->m_id;
          label_map[std::string(pimg->label)] = stPtr;
          pos_map[imp3Dpos(imgpt.x, imgpt.y, imgpt.z)] = stPtr;
        }
        break;
      case img_XSECT:
        lmi = label_map.find(pimg->label);
        if (lmi != label_map.end()) {
          stPtr = lmi->second;
          stPtr->SetFlag(LXFILE_STATION_FLAG_HAS_WALLS, true);
        }
        break;
      case img_BAD:
        this->m_error = "invalid file format";
        return;
    }
  } while (result != img_STOP);

  img_rewind(pimg);
  stPtr = NULL;

  do {
    result = img_read_item(pimg, &imgpt);
    switch (result) {
      case img_MOVE:
      case img_LINE:
        if ((!hasPT) && (result == img_LINE)) {
          result = img_MOVE;
        }
        pmi = pos_map.find(imp3Dpos(imgpt.x, imgpt.y, imgpt.z));
        if (pmi != pos_map.end()) {
          stPtr = pmi->second;
        } else {
          stPtr = this->NewStation();
          stPtr->m_c[0] = imgpt.x;
          stPtr->m_c[1] = imgpt.y;
          stPtr->m_c[2] = imgpt.z;
          stPtr->m_surveyId = tmpSurvey->m_id;
        }
        hasPT = true;
        if (result == img_LINE) {
          shPtr = this->NewShot();
          shPtr->m_from = stPtrPrev->m_id;
          shPtr->m_to = stPtr->m_id;
          shPtr->m_surveyId = tmpSurvey->m_id;
          shPtr->m_sectionType = LXFILE_SHOT_SECTION_NONE;
          if ((pimg->flags & img_FLAG_SURFACE) != 0)  {
            shPtr->SetFlag(LXFILE_SHOT_FLAG_SURFACE, true);
          }
          if ((pimg->flags & img_FLAG_DUPLICATE) != 0)  {
            shPtr->SetFlag(LXFILE_SHOT_FLAG_DUPLICATE, true);
          }
          if ((pimg->flags & img_FLAG_SPLAY) != 0)  {
            shPtr->SetFlag(LXFILE_SHOT_FLAG_SPLAY, true);
          }
          if (stPtr->GetFlag(LXFILE_STATION_FLAG_HAS_WALLS) || stPtrPrev->GetFlag(LXFILE_STATION_FLAG_HAS_WALLS)) {
            shPtr->SetFlag(LXFILE_SHOT_FLAG_NOT_LRUD, true);
          }
        }
        stPtrPrev = stPtr;
        break;

      case img_XSECT:
        lmi = label_map.find(pimg->label);
        if (lmi != label_map.end()) {
          stPtrLRUD = lmi->second;
          lrud[0] = pimg->l;
          lrud[1] = pimg->r;
          lrud[2] = pimg->u;
          lrud[3] = pimg->d;
          lrudOK = lxFile__CheckLRUD(lrud[0], lrud[1], lrud[2], lrud[3], 2.0, 5.0);
          if (lrudOK) {
            stPtr->SetFlag(LXFILE_STATION_FLAG_HAS_WALLS, true);
          }
          if ((stPtrLRUDPrev != NULL) && lrudOK && lrudOKPrev) {
            shPtr = this->NewShot();
            shPtr->m_from = stPtrLRUDPrev->m_id;
            shPtr->m_to = stPtrLRUD->m_id;
            shPtr->m_fLRUD[0] = lrudPrev[0];
            shPtr->m_fLRUD[1] = lrudPrev[1];
            shPtr->m_fLRUD[2] = lrudPrev[2];
            shPtr->m_fLRUD[3] = lrudPrev[3];
            shPtr->m_tLRUD[0] = lrud[0];
            shPtr->m_tLRUD[1] = lrud[1];
            shPtr->m_tLRUD[2] = lrud[2];
            shPtr->m_tLRUD[3] = lrud[3];
            shPtr->m_surveyId = tmpSurvey->m_id;
            shPtr->m_sectionType = LXFILE_SHOT_SECTION_OVAL;
            shPtr->SetFlag(LXFILE_SHOT_FLAG_NOT_VISIBLE, true);
            shPtr->SetFlag(LXFILE_SHOT_FLAG_NOT_LRUD, true);
          }
        } else {
          stPtrLRUD = NULL;
          lrud[0] = lrud[1] = lrud[2] = lrud[3] = -999.0;
          lrudOK = false;
          lrud[0] = -999.0;
        }
        lrudPrev[0] = lrud[0];
        lrudPrev[1] = lrud[1];
        lrudPrev[2] = lrud[2];
        lrudPrev[3] = lrud[3];
        stPtrLRUDPrev = stPtrLRUD;
        lrudOKPrev = lrudOK;
        break;

      case img_XSECT_END:
        stPtrLRUDPrev = NULL;
        break;
      case img_BAD:
        this->m_error = "invalid file format";
        return;
    }

  } while (result != img_STOP);
  img_close(pimg);

}



struct missingShot {
  lxFileSizeT f, t;
  double length;
};


struct missingStation {
  lxVec position;
  double average, sum, count;
};


void lxFile::InterpolateMissingLRUD()
{

  if (this->m_shots.size() == 0) return;
  if (this->m_stations.size() == 0) return;

  std::map<lxVec, lxFileSizeT> stmap;
  std::vector<lxFileStation*> osts;
  std::vector<missingStation> stations;
  std::list<missingShot> shots;
  lxFileSizeT ns, i;


  // 0. vytvorit vector originalnych stations
  lxFileStation_list::iterator osti;
  osts.resize(this->m_stations.size());
  for (osti = this->m_stations.begin(); osti != this->m_stations.end(); osti++) {
    osts[osti->m_id] = &(*osti);
  }


  // 1. vytvorit zoznam identickych bodov a zamer medzi nimi
  ns = 0;
  std::list<lxFileShot>::iterator shi;
  std::map<lxVec, lxFileSizeT>::iterator stmi;
  lxFileStation * st;
  missingStation tst;
  missingShot ts;
  lxVec fp, tp;
  for (shi = this->m_shots.begin(); shi != this->m_shots.end(); shi++) {
    if (!(shi->GetFlag(LXFILE_SHOT_FLAG_SURFACE) || shi->GetFlag(LXFILE_SHOT_FLAG_DUPLICATE) || shi->GetFlag(LXFILE_SHOT_FLAG_NOT_VISIBLE) || shi->GetFlag(LXFILE_SHOT_FLAG_NOT_LRUD))) {

      // from
      st = osts[shi->m_from];
      fp = lxVec(st->m_c[0],st->m_c[1],st->m_c[2]);
      stmi = stmap.find(fp);
      if (stmi == stmap.end()) {
        ts.f = ns;
        stmap[fp] = ts.f;
        tst.position = fp;
        stations.push_back(tst);
        ns++;
      } else {
        ts.f = stmi->second;
      }

      // to
      st = osts[shi->m_to];
      tp = lxVec(st->m_c[0],st->m_c[1],st->m_c[2]);
      stmi = stmap.find(tp);
      if (stmi == stmap.end()) {
        ts.t = ns;
        stmap[tp] = ts.t;
        ns++;
        tst.position = tp;
        stations.push_back(tst);
      } else {
        ts.t = stmi->second;
      }

      ts.length = (tp - fp).Length();
      shots.push_back(ts);
    }
  }


  // 2. zratat priemerne dlzky
  if (ns == 0) return;
  std::list<missingShot>::iterator shli;
  for(i = 0; i < ns; i++) {
    stations[i].sum = 0.0;
    stations[i].count = 0.0;
  }
  for(shli = shots.begin(); shli != shots.end(); shli++) {
    stations[shli->f].sum += shli->length;
    stations[shli->f].count += 1.0;
    stations[shli->t].sum += shli->length;
    stations[shli->t].count += 1.0;
  }
  for(i = 0; i < ns; i++) {
    stations[i].average = stations[i].sum / stations[i].count;
    stations[i].sum = stations[i].average;
    stations[i].count = 1.0;
  }


  // 3. urobit klzavy priemer
  for(shli = shots.begin(); shli != shots.end(); shli++) {
    stations[shli->f].sum += stations[shli->t].average;
    stations[shli->t].sum += stations[shli->f].average;
    stations[shli->f].count += 1.0;
    stations[shli->t].count += 1.0;
  }
  double avg;
  for(i = 0; i < ns; i++) {
    avg = 0.25 * stations[i].sum / stations[i].count;
    if (avg < 0.5) avg = 0.3048;
    stations[i].average = avg;
  }


  // 4. interpolovat LRUD vsade kde ho nemame a neni surface
  for (shi = this->m_shots.begin(); shi != this->m_shots.end(); shi++) {
    if (!(shi->GetFlag(LXFILE_SHOT_FLAG_SURFACE) || shi->GetFlag(LXFILE_SHOT_FLAG_DUPLICATE) || shi->GetFlag(LXFILE_SHOT_FLAG_NOT_VISIBLE) || shi->GetFlag(LXFILE_SHOT_FLAG_NOT_LRUD))) {
      if (shi->m_sectionType == LXFILE_SHOT_SECTION_NONE) {
        shi->m_sectionType = LXFILE_SHOT_SECTION_OVAL;
        st = osts[shi->m_from];
        fp = lxVec(st->m_c[0],st->m_c[1],st->m_c[2]);
        i = stmap[fp];
        avg = stations[i].average;
        shi->m_fLRUD[0] = avg;
        shi->m_fLRUD[1] = avg;
        shi->m_fLRUD[2] = avg;
        shi->m_fLRUD[3] = lxMin(avg, 1.5);
        st = osts[shi->m_to];
        tp = lxVec(st->m_c[0],st->m_c[1],st->m_c[2]);
        i = stmap[tp];
        avg = stations[i].average;
        shi->m_tLRUD[0] = avg;
        shi->m_tLRUD[1] = avg;
        shi->m_tLRUD[2] = avg;
        shi->m_tLRUD[3] = lxMin(avg, 1.5);
      }
    }
  }

}


lxFileSizeT lxFileStation::Save(lxFileBuff & ptr)
{
  lxFileSizeT s(0);
  s += this->m_id.Save(ptr);
  s += this->m_surveyId.Save(ptr);
  s += this->m_namePtr.Save(ptr);
  s += this->m_commentPtr.Save(ptr);
  s += this->m_flags.Save(ptr);
  s += this->m_c[0].Save(ptr);
  s += this->m_c[1].Save(ptr);
  s += this->m_c[2].Save(ptr);
  return s;
}


lxFileSizeT lxFileStation::Load(lxFileBuff & ptr)
{
  lxFileSizeT s(0);
  s += this->m_id.Load(ptr);
  s += this->m_surveyId.Load(ptr);
  s += this->m_namePtr.Load(ptr);
  s += this->m_commentPtr.Load(ptr);
  s += this->m_flags.Load(ptr);
  s += this->m_c[0].Load(ptr);
  s += this->m_c[1].Load(ptr);
  s += this->m_c[2].Load(ptr);
  return s;
}


void lxFileStation::SetFlag(int flag, bool value)
{
  if (value)
    this->m_flags |= flag;
  else
    this->m_flags &= ~flag;
}


bool lxFileStation::GetFlag(int flag)
{
  return ((this->m_flags & flag) != 0);
}




lxFileSizeT lxFileShot::Save(lxFileBuff & ptr)
{
  lxFileSizeT s(0);
  s += this->m_from.Save(ptr);
  s += this->m_to.Save(ptr);
  s += this->m_fLRUD[0].Save(ptr);
  s += this->m_fLRUD[1].Save(ptr);
  s += this->m_fLRUD[2].Save(ptr);
  s += this->m_fLRUD[3].Save(ptr);
  s += this->m_tLRUD[0].Save(ptr);
  s += this->m_tLRUD[1].Save(ptr);
  s += this->m_tLRUD[2].Save(ptr);
  s += this->m_tLRUD[3].Save(ptr);
  s += this->m_flags.Save(ptr);
  s += this->m_sectionType.Save(ptr);
  s += this->m_surveyId.Save(ptr);
  s += this->m_threshold.Save(ptr);
  return s;
}


lxFileSizeT lxFileShot::Load(lxFileBuff & ptr)
{
  lxFileSizeT s(0);
  s += this->m_from.Load(ptr);
  s += this->m_to.Load(ptr);
  s += this->m_fLRUD[0].Load(ptr);
  s += this->m_fLRUD[1].Load(ptr);
  s += this->m_fLRUD[2].Load(ptr);
  s += this->m_fLRUD[3].Load(ptr);
  s += this->m_tLRUD[0].Load(ptr);
  s += this->m_tLRUD[1].Load(ptr);
  s += this->m_tLRUD[2].Load(ptr);
  s += this->m_tLRUD[3].Load(ptr);
  s += this->m_flags.Load(ptr);
  s += this->m_sectionType.Load(ptr);
  s += this->m_surveyId.Load(ptr);
  s += this->m_threshold.Load(ptr);
  return s;
}


void lxFileShot::SetFlag(int flag, bool value)
{
  if (value)
    this->m_flags |= flag;
  else
    this->m_flags &= ~flag;
}


bool lxFileShot::GetFlag(int flag)
{
  return ((this->m_flags & flag) != 0);
}


lxFileSizeT lxFileScrap::Save(lxFileBuff & ptr)
{
  lxFileSizeT s(0);
  s += this->m_id.Save(ptr);
  s += this->m_surveyId.Save(ptr);
  s += this->m_numPoints.Save(ptr);
  s += this->m_pointsPtr.Save(ptr);
  s += this->m_num3Angles.Save(ptr);
  s += this->m_3AnglesPtr.Save(ptr);
  return s;
}


lxFileSizeT lxFileScrap::Load(lxFileBuff & ptr)
{
  lxFileSizeT s(0);
  s += this->m_id.Load(ptr);
  s += this->m_surveyId.Load(ptr);
  s += this->m_numPoints.Load(ptr);
  s += this->m_pointsPtr.Load(ptr);
  s += this->m_num3Angles.Load(ptr);
  s += this->m_3AnglesPtr.Load(ptr);
  return s;
}


lxFileSizeT lxFileSurface::Save(lxFileBuff & ptr)
{
  lxFileSizeT s(0);
  s += this->m_id.Save(ptr);
  s += this->m_width.Save(ptr);
  s += this->m_height.Save(ptr);
  s += this->m_dataPtr.Save(ptr);
  s += this->m_calib[0].Save(ptr);
  s += this->m_calib[1].Save(ptr);
  s += this->m_calib[2].Save(ptr);
  s += this->m_calib[3].Save(ptr);
  s += this->m_calib[4].Save(ptr);
  s += this->m_calib[5].Save(ptr);
  return s;
}


lxFileSizeT lxFileSurface::Load(lxFileBuff & ptr)
{
  lxFileSizeT s(0);
  s += this->m_id.Load(ptr);
  s += this->m_width.Load(ptr);
  s += this->m_height.Load(ptr);
  s += this->m_dataPtr.Load(ptr);
  s += this->m_calib[0].Load(ptr);
  s += this->m_calib[1].Load(ptr);
  s += this->m_calib[2].Load(ptr);
  s += this->m_calib[3].Load(ptr);
  s += this->m_calib[4].Load(ptr);
  s += this->m_calib[5].Load(ptr);
  return s;
}


lxFileSizeT lxFileSurfaceBitmap::Save(lxFileBuff & ptr)
{
  lxFileSizeT s(0);
  s += this->m_surfaceId.Save(ptr);
  s += this->m_type.Save(ptr);
  s += this->m_dataPtr.Save(ptr);
  s += this->m_calib[0].Save(ptr);
  s += this->m_calib[1].Save(ptr);
  s += this->m_calib[2].Save(ptr);
  s += this->m_calib[3].Save(ptr);
  s += this->m_calib[4].Save(ptr);
  s += this->m_calib[5].Save(ptr);
  return s;
}


lxFileSizeT lxFileSurfaceBitmap::Load(lxFileBuff & ptr)
{
  lxFileSizeT s(0);
  s += this->m_surfaceId.Load(ptr);
  s += this->m_type.Load(ptr);
  s += this->m_dataPtr.Load(ptr);
  s += this->m_calib[0].Load(ptr);
  s += this->m_calib[1].Load(ptr);
  s += this->m_calib[2].Load(ptr);
  s += this->m_calib[3].Load(ptr);
  s += this->m_calib[4].Load(ptr);
  s += this->m_calib[5].Load(ptr);
  return s;
}

bool lxFile::HasAnyWalls()
{
  if (this->m_scraps.size() > 0)
    return true;
  std::list<lxFileShot>::iterator shi;
  for (shi = this->m_shots.begin(); shi != this->m_shots.end(); shi++) {
    if (!(shi->GetFlag(LXFILE_SHOT_FLAG_SURFACE) || shi->GetFlag(LXFILE_SHOT_FLAG_DUPLICATE) || shi->GetFlag(LXFILE_SHOT_FLAG_NOT_LRUD))) {
      if (shi->m_sectionType != LXFILE_SHOT_SECTION_NONE) {
        return true;
      }
    }
  }
  return false;
}





