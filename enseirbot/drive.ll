; cette fonction est la fonction principale a generer. Vous devez repecter cette
; interface.
define void @drive(i32 %index, %struct.CarElt* %car, %struct.Situation* %s) {
	%ctrl		= getelementptr %struct.CarElt* %car, i32 0, i32 5
	%public_car	= getelementptr %struct.CarElt* %car, i32 0, i32 2
	%pos		= getelementptr %struct.tPublicCar* %public_car, i32 0, i32 3
	%seg.addr	= getelementptr %struct.tTrkLocPos* %pos, i32 0, i32 0
	%seg		= load %struct.trackSeg** %seg.addr
; lecture de la position du volant, de la commande d'accelerateur, de frein, de debrayage, de levier de vitesse
	%steer		= getelementptr %struct.tCarCtrl* %ctrl, i32 0, i32 0
	%accelCmd	= getelementptr %struct.tCarCtrl* %ctrl, i32 0, i32 1
	%brakeCmd	= getelementptr %struct.tCarCtrl* %ctrl, i32 0, i32 2
	%clutchCmd	= getelementptr %struct.tCarCtrl* %ctrl, i32 0, i32 3
	%gear		= getelementptr %struct.tCarCtrl* %ctrl, i32 0, i32 4
; lecture de l'angle que fait la route et de l'angle que fait la voiture (par rapport a la position de depart dans la course)
	%road_angle = call float @get_track_angle(%struct.tTrkLocPos* %pos)
	%car_angle	= call float @get_car_yaw(%struct.CarElt* %car)
; on calcule la différence et on normalise l'angle
	%angle		= fsub float %road_angle, %car_angle
	%nangle		= call float @norm_pi_pi(float %angle)
; on change l'angle du volant en fonction de la position de la voiture par rapport au milieu de la piste
	%posmid		= call float @get_pos_to_middle(%struct.tTrkLocPos* %pos)
	%width		= call float @get_track_seg_width(%struct.trackSeg* %seg)
	%corr		= fdiv float %posmid, %width
	%cangle		= fsub float %nangle, %corr
; on met a jour le volant, l'accelerateur, les freins et le levier de vitesse
	store float %cangle, float* %steer
	store float 0.750000e+00, float* %accelCmd
	store float 0.000000e+00, float* %brakeCmd
	store float 0.000000e+00, float* %clutchCmd
	store i32 1, i32* %gear

	ret void
}
; fonctions prefinies dans le fichier enseirbot.cpp 
declare float @norm_pi_pi(float %a)
declare float @get_track_angle(%struct.tTrkLocPos*)
declare float @get_pos_to_right(%struct.tTrkLocPos*)
declare float @get_pos_to_middle(%struct.tTrkLocPos*)
declare float @get_pos_to_left(%struct.tTrkLocPos*)
declare float @get_pos_to_start(%struct.tTrkLocPos*)
declare float @get_track_seg_length(%struct.trackSeg*)
declare float @get_track_seg_width(%struct.trackSeg*)
declare float @get_track_seg_start_width(%struct.trackSeg*)
declare float @get_track_seg_end_width(%struct.trackSeg*)
declare float @get_track_seg_radius(%struct.trackSeg*)
declare float @get_track_seg_right_radius(%struct.trackSeg*)
declare float @get_track_seg_left_radius(%struct.trackSeg*)
declare float @get_track_seg_arc(%struct.trackSeg*)
declare %struct.trackSeg* @get_track_seg_next(%struct.trackSeg*)
declare float @get_car_yaw(%struct.CarElt*)
