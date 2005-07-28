/*
    Suppress points based on high Degree of Precision (DOP) values.

    Copyright (C) 2005 Robert Lipe, robertlipe@usa.net

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111 USA

 */
#include <stdio.h>
#include "defs.h"

extern queue waypt_head;

static char *hdopopt = NULL;
static char *vdopopt = NULL;
static char *andopt = NULL;
static double hdopf;
static double vdopf;

static
arglist_t fix_args[] = {
	{"hdop", &hdopopt, "Suppress waypoints with higher hdop",
		"-1.0", ARGTYPE_BEGIN_REQ | ARGTYPE_FLOAT},
	{"vdop", &vdopopt, "Suppress waypoints with higher vdop",
		"-1.0", ARGTYPE_END_REQ | ARGTYPE_FLOAT},
	{"hdopandvdop", &andopt, "Link hdop and vdop supression with AND",
		NULL, ARGTYPE_BOOL},
	{0, 0, 0, 0, 0}
};

static void
fix_process_track(const route_head *trk)
{
	waypoint * waypointp;
	queue *elem, *tmp;
	
	QUEUE_FOR_EACH((queue *)&trk->waypoint_list, elem, tmp) {
		
		int del = 0;
		int delh = 0;
		int delv = 0;

		waypointp = (waypoint *)elem;
		
		if ((hdopf >= 0.0) && (waypointp->hdop > hdopf))
			delh = 1;
		if ((vdopf >= 0.0) && (waypointp->vdop > vdopf))
			delv = 1;
		
		if (andopt)
			del = delh && delv;
		else
			del = delh || delv;

		if (del) {
			waypt_del(waypointp);
			waypt_free(waypointp);
		}

	}
}

void
fix_process(void)
{
	waypoint * waypointp;
	queue *elem, *tmp;
	extern queue waypt_head;
	
	// Filter waypoints

	QUEUE_FOR_EACH(&waypt_head, elem, tmp) {
		
		int del = 0;
		int delh = 0;
		int delv = 0;

		waypointp = (waypoint *)elem;
		
		if ((hdopf >= 0.0) && (waypointp->hdop > hdopf))
			delh = 1;
		if ((vdopf >= 0.0) && (waypointp->vdop > vdopf))
			delv = 1;
		
		if (andopt)
			del = delh && delv;
		else
			del = delh || delv;

		if (del) {
			waypt_del(waypointp);
			waypt_free(waypointp);
		}

	}
	
	// Filter tracks
	track_disp_all(fix_process_track, NULL, NULL);
	
	// And routes
	route_disp_all(fix_process_track, NULL, NULL);
	
}

void
fix_init(const char *args) 
{
	if (hdopopt)
		hdopf = atof(hdopopt);
	else
		hdopf = -1.0;
	if (vdopopt)
		vdopf = atof(vdopopt);
	else
		vdopf = -1.0;
}

void
fix_deinit(void) 
{
}

filter_vecs_t fix_vecs = {
	fix_init,
	fix_process,
	fix_deinit,
	NULL,
	fix_args
};