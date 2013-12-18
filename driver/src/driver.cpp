
/***************************************************************************

    file                 : enseirBOT.cpp
    created              : Tue Nov 19 14:04:20 CET 2013
    copyright            : (C) 2002 Bertrand Putigny

 ***************************************************************************/

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <math.h>

#include <tgf.h> 
#include <track.h> 
#include <car.h> 
#include <raceman.h> 
#include <robottools.h>
#include <robot.h>

static tTrack	*curTrack;

static void initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation *s); 
static void newrace(int index, tCarElt* car, tSituation *s); 
extern "C" void drive(int index, tCarElt* car, tSituation *s); 
static void endrace(int index, tCarElt *car, tSituation *s);
static void shutdown(int index);
static int  InitFuncPt(int index, void *pt); 


/* 
 * Module entry point  
 */ 
extern "C" int 
enseirBOT(tModInfo *modInfo) {
    memset(modInfo, 0, 10*sizeof(tModInfo));

    modInfo->name    = strdup("enseirBOT");		/* name of the module (short) */
    modInfo->desc    = strdup("");	/* description of the module (can be long) */
    modInfo->fctInit = InitFuncPt;		/* init function */
    modInfo->gfId    = ROB_IDENT;		/* supported framework version */
    modInfo->index   = 1;

    return 0; 
} 

/* Module interface initialization. */
static int 
InitFuncPt(int index, void *pt) { 
    tRobotItf *itf  = (tRobotItf *)pt; 

    itf->rbNewTrack = initTrack; /* Give the robot the track view called */ 
				 /* for every track change or new race */ 
    itf->rbNewRace  = newrace; 	 /* Start a new race */
    itf->rbDrive    = drive;	 /* Drive during race */
    itf->rbPitCmd   = NULL;
    itf->rbEndRace  = endrace;	 /* End of the current race */
    itf->rbShutdown = shutdown;	 /* Called before the module is unloaded */
    itf->index      = index; 	 /* Index used if multiple interfaces */
    return 0; 
} 

/* Called for every track change or new race. */
static void  
initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation *s) { 
    curTrack = track;
    *carParmHandle = NULL; 
} 

/* Start a new race. */
static void  
newrace(int index, tCarElt* car, tSituation *s) { 
} 


/* End of the current race */
static void
endrace(int index, tCarElt *car, tSituation *s) {
}

/* Called before the module is unloaded */
static void
shutdown(int index) {}

extern "C" float norm_pi_pi(float a) {
	float r = a;
	while (r > PI)
		r -= 2*PI;
	while (r < -PI)
		r += 2*PI;
	return r;
}

/* Car position */
extern "C" float get_track_angle(tTrkLocPos *pos) {
	return RtTrackSideTgAngleL(pos);
}
extern "C" float get_pos_to_right(tTrkLocPos *pos) {
	return pos->toRight;
}
extern "C" float get_pos_to_middle(tTrkLocPos *pos) {
	return pos->toMiddle;
}
extern "C" float get_pos_to_left(tTrkLocPos *pos) {
	return pos->toLeft;
}
extern "C" float get_pos_to_start(tTrkLocPos *pos) {
	return pos->toStart;
}

/* Track segment geomerty */
extern "C" float get_track_seg_length(tTrackSeg *seg) {
	return seg->length;
}
extern "C" float get_track_seg_width(tTrackSeg *seg) {
	return seg->width;
}
extern "C" float get_track_seg_start_width(tTrackSeg *seg) {
	return seg->startWidth;
}
extern "C" float get_track_seg_end_width(tTrackSeg *seg) {
	return seg->endWidth;
}
extern "C" float get_track_seg_radius(tTrackSeg *seg) {
	return seg->radius;
}
extern "C" float get_track_seg_right_radius(tTrackSeg *seg) {
	return seg->radiusr;
}
extern "C" float get_track_seg_left_radius(tTrackSeg *seg) {
	return seg->radiusl;
}
extern "C" float get_track_seg_arc(tTrackSeg *seg) {
	return seg->arc;
}
extern "C" tTrackSeg *get_track_seg_next(tTrackSeg *seg) {
	return seg->next;
}

/* Car */
extern "C" float get_car_yaw(tCarElt *car) {
	return car->_yaw;
}
