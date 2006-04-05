#include "lxFile.h"

// Standard libraries
#ifndef LXDEPCHECK
#include <stdlib.h>
#include <stdio.h>
#endif  
//LXDEPCHECK - standart libraries


lxFileDataPtr::lxFileDataPtr()
{
  this->Clear();
}

void lxFileDataPtr::Clear()
{
  this->m_position = 0;
  this->m_size = 0;
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

void lxFileData::Copy(size_t size, const void * src)
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
  //rf = tmpfile();
  rf = fopen("TMPFILE.JPG","wb");
  if (rf != NULL) {
    fwrite(&(((char *)this->m_data)[ptr.m_position]), 1, ptr.m_size, rf);
    //fseek(rf, 0, SEEK_SET);
    fclose(rf);
    rf = fopen("TMPFILE.JPG","rb");
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

void lxFileData::BuffResize(size_t size)
{
  size_t nsize;
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
  size_t strln;
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
    size_t fsz = ftell(xf);
    fseek(xf, 0, SEEK_SET);
    if (fsz > 0) {
      char * cdata = new char [fsz];
      fread((void *) cdata, 1, fsz, xf);
      res = this->AppendData(cdata, fsz);
      delete [] cdata;
    }
    fclose(xf);
  }
  return res;
}



lxFileDataPtr lxFileData::AppendData(const void * data, size_t size)
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

  this->m_stations.clear();
  this->m_stationsData.Clear();

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
};


void lxFile::ExportLOX(const char * fn)
{
  
  this->m_error.clear();

  this->m_file = fopen(fn,"wb");
  if (this->m_file == NULL) {
    this->m_error = "unable to open file for output";
    return;
  }
  
  // write surveys
  lxFileChunkHdr chunkHdr;
  bool writeErr;

  writeErr = false;
  size_t x, size;

#define lxFileExportItem(expID, expClass, expRecs, expData) \
  size = this->expRecs.size(); \
  if ((!writeErr) && (size > 0)) { \
    expClass * tmpData = new expClass [size]; \
    expClass##_list::iterator tmpIter = this->expRecs.begin(); \
    for (x = 0; x < size; x++) { \
      tmpData[x] = *tmpIter; \
      tmpIter++; \
    } \
    chunkHdr.m_type = expID; \
    chunkHdr.m_recSize = sizeof(expClass); \
    chunkHdr.m_recCount = size; \
    chunkHdr.m_dataSize = this->expData.m_size; \
    if (fwrite(&chunkHdr, sizeof(chunkHdr), 1, this->m_file) != 1) \
      writeErr = true; \
    if (fwrite(tmpData, chunkHdr.m_recSize, size, this->m_file) != size) \
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

  // read surveys
  size_t i, orig_size, orig_survey_id, orig_station_id;

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

  lxFileChunkHdr chunkHdr;
  bool readErr = false;
  while ((!readErr) && (!feof(this->m_file)) && (fread(&chunkHdr, sizeof(lxFileChunkHdr), 1, this->m_file) == 1)) {
    switch (chunkHdr.m_type) {

      /*
      case LXFILE_CHUNK_SURVEY:
        if (chunkHdr.m_recCount > 0) {
          orig_size = this->m_surveysData.m_size;
          lxFileSurvey * tmpRecs = new lxFileSurvey [chunkHdr.m_recCount];
          if (fread(tmpRecs, sizeof(lxFileSurvey), chunkHdr.m_recCount, this->m_file) != chunkHdr.m_recCount)
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
          impClass * tmpRecs = new impClass [chunkHdr.m_recCount]; \
          if (fread(tmpRecs, sizeof(impClass), chunkHdr.m_recCount, this->m_file) != chunkHdr.m_recCount) \
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
            for(i = 0; i < chunkHdr.m_recCount; i++) {

#define lxFileEndImportItem() \
            } \
          } \
          delete [] tmpRecs; \
        } \
        break; 

      lxFileStartImportItem(LXFILE_CHUNK_SURVEY, lxFileSurvey, m_surveysData)
        tmpRecs[i].m_id += orig_survey_id;
        tmpRecs[i].m_namePtr.m_position += orig_size;
        tmpRecs[i].m_titlePtr.m_position += orig_size;
        this->m_surveys.push_back(tmpRecs[i]);
      lxFileEndImportItem()

      lxFileStartImportItem(LXFILE_CHUNK_STATION, lxFileStation, m_stationsData)
        tmpRecs[i].m_id += orig_station_id;
        tmpRecs[i].m_surveyId += orig_survey_id;
        tmpRecs[i].m_namePtr.m_position += orig_size;
        tmpRecs[i].m_commentPtr.m_position += orig_size;
        this->m_stations.push_back(tmpRecs[i]);
      lxFileEndImportItem()

      lxFileStartImportItem(LXFILE_CHUNK_SHOT, lxFileShot, m_shotsData)
        tmpRecs[i].m_from += orig_station_id;
        tmpRecs[i].m_to += orig_station_id;
        tmpRecs[i].m_surveyId += orig_survey_id;
        this->m_shots.push_back(tmpRecs[i]);
      lxFileEndImportItem()

      lxFileStartImportItem(LXFILE_CHUNK_SCRAP, lxFileScrap, m_scrapsData)
        tmpRecs[i].m_pointsPtr.m_position += orig_size;
        tmpRecs[i].m_3AnglesPtr.m_position += orig_size;
        tmpRecs[i].m_surveyId += orig_survey_id;
        this->m_scraps.push_back(tmpRecs[i]);
      lxFileEndImportItem()

      lxFileStartImportItem(LXFILE_CHUNK_SURFACE, lxFileSurface, m_surfacesData)
        tmpRecs[i].m_dataPtr.m_position += orig_size;
        this->m_surfaces.push_back(tmpRecs[i]);
      lxFileEndImportItem()

      lxFileStartImportItem(LXFILE_CHUNK_SURFACEBMP, lxFileSurfaceBitmap, m_surfaceBitmapsData)
        tmpRecs[i].m_dataPtr.m_position += orig_size;
        this->m_surfaceBitmaps.push_back(tmpRecs[i]);
      lxFileEndImportItem()

      default:
        readErr = true;
    }
  }

  fclose(this->m_file);
}

