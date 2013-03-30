(*
  output.ml: main function to generate multiple instance files for
  the CrewPlanning domain
*)

(* number of exercise equipment *)
let nExerEquip = ref 1
(* which day which CrewMember needs mcs exam *)
and mcsDay = Array.make_matrix 51 51 false
(* which day we need rpcm to get done *)
and rpcmDay = Array.make 51 false
(* which day we need to change the filter *)
and filterDay = Array.make 51 false		
(* how many payload activities for each day *)
and nPayloadAct = Array.make 51 0
(* available time for each crew member each day *)
and crew = ref 0
and day = ref 0
and utilization = ref 25 
			     

let total_time_day d =
  (** find the total available time for day up until day [d] is done;
    the total available time equal to total time (1440minute/day) minus:
    (1) daily-routine for each member (915); (2) finish rpcm until that
    day (420/rpcm); (3) change filter (60/filter); (4) mcs (60/crew/time);
    (5) payload activities up until the day before [d] *)
  let tt = ref 0 in
    for i = 1 to !day do
      for j = 1 to !crew do
	tt := !tt + 525;
	if mcsDay.(i).(j) then tt := !tt - 60;
      done;
      if rpcmDay.(i) then tt := !tt - 420;
      if filterDay.(i) then tt := !tt - 60;
      if i < d then tt := !tt - (60 * nPayloadAct.(i))	
    done;
    !tt



let set_parameters () =
  (** function to set the parameters above. Note: 915 = daily routine time *)
    nExerEquip := (Random.int !crew) + 1;
    (* setup the mcs exam; need to be done for each member every 24-48 hours *)
    for i = 1 to !day do
      for j = 1 to !crew do
	if j = 1 then
	  mcsDay.(i).(j) <- Random.bool ()
	else
	  if mcsDay.(i).(j-1) = true then
	    mcsDay.(i).(j) <- Random.bool ()
	  else
	    mcsDay.(i).(j) <- true
      done
    done;
    (* randomly setup rpcm *)
    for i = 1 to !day do
      rpcmDay.(i) <- Random.bool ()
    done;
    (* randomly setup if a filter needs to be changed in a given day *)
    for i = 1 to !day do
      filterDay.(i) <- Random.bool ()
    done;
    (* calculate the number of activities to be dued today due to
       available time and how we want to  utilize that *)
    for i = 1 to !day do
      nPayloadAct.(i) <- ((total_time_day i) * !utilization) / (100 * 60)
    done


let gen_objects ch =
  let flag = ref false in
    Utils.pf ch "(:objects\n\t";
    for i = 0 to !day do
      Utils.pf ch "d%d " i
    done;
    Utils.pf ch "- Day\n\n\t";
    for i = 1 to !crew do
      Utils.pf ch "c%d " i
    done;
    Utils.pf ch "- CrewMember\n\t";
    for i = 1 to !crew do
      Utils.pf ch "fs%d " i
    done;
    Utils.pf ch "- FastingState\n\t";
    for i = 1 to !crew do
      Utils.pf ch "mcs%d " i
    done;
    Utils.pf ch "- ProcessState\n\n\t";
    Utils.pf ch "spaceshipFilter - FilterState\n\n\t";
    flag := false;
    for i = 1 to !day do
      if rpcmDay.(i) then
	(Utils.pf ch "rpcm%d " i;
	 flag := true)
    done;
    if !flag then
      Utils.pf ch "- ProcessState\n\n\t";
    flag := false;
    for i = 1 to !day do
      if nPayloadAct.(i) > 0 then
	for j = 1 to nPayloadAct.(i) do
	  Utils.pf ch "pa%d_%d " i j;
	  flag := true
	done
    done;
    Utils.pf ch "- PayloadAct\n\n\t";
    for i = 1 to !nExerEquip do
      Utils.pf ch "e%d " i
    done;
    Utils.pf ch "- ExerEquipment\n)\n\n"


let gen_init ch =
  (** generate the initial state based on the objects set above *)
  (* initialize the crew members *)
  Utils.pf ch "(:init\n\t";
  for i = 1 to !crew do
    Utils.pf ch "(currentday c%d d0)\n\t" i;
    Utils.pf ch "(done_sleep c%d d0)\n\t" i;
    Utils.pf ch "(available c%d)\n\t" i;
    Utils.pf ch "(fasting_window fs%d c%d)\n\t" i i;
    Utils.pf ch "(fasting fs%d)\n\t" i;
    Utils.pf ch "(medicalstate mcs%d c%d)\n\n\t" i i 
  done;
  (* initialize the days *)
  Utils.pf ch "(initiated d1)\n\t";
  for i = 1 to !day do
    Utils.pf ch "(next d%d d%d)\n\t" (i-1) i
  done;
  Utils.pf ch "\n\t";
  (* initialize exercise equipments *)
  for i = 1 to !nExerEquip do
    Utils.pf ch "(unused e%d)\n\t" i
  done;
  Utils.pf ch "\n";
  (* initiliaze rpcm *)
  for i = 1 to !day do
    if rpcmDay.(i) = true then Utils.pf ch "\t(rpcm rpcm%d)\n" i
  done;
  Utils.pf ch ")\n\n"


let gen_goal ch =
  Utils.pf ch "(:goal\n(and\n";
  (* daily routine: everyting until sleeping is done *)
  for i = 1 to !crew do
    for j = 1 to !day do
      Utils.pf ch "\t(done_sleep c%d d%d)\n" i j
    done
  done;
  Utils.pf ch "\n";
  (* finish individual medical conferences *)
  for i = 1 to !day do
    for j = 1 to !crew do
      if mcsDay.(i).(j) = true then Utils.pf ch "\t(finished mcs%d d%d)\n" j i
    done
  done;
  Utils.pf ch "\n";
  (* change spaceship filter *)
  for i = 1 to !day do
    if filterDay.(i) = true then Utils.pf ch "\t(changed spaceshipFilter d%d)\n" i
  done;
  Utils.pf ch "\n";
  (* finish RPCM *)
  for i = 1 to !day do
    if rpcmDay.(i) = true then Utils.pf ch "\t(finished rpcm%d d%d)\n" i i
  done;
  Utils.pf ch "\n";
  (* finish payload activities *)
  for i = 1 to !day do
    if nPayloadAct.(i) > 0 then
      for j = 1 to nPayloadAct.(i) do
	Utils.pf ch "\t(payload_act_completed pa%d_%d d%d)\n" i j i
      done
  done;
  Utils.pf ch ")\n)\n"



let create_instance d c ux ch =
  (** generate the problem file with number of [day] and [crew] pre-specified *)
  crew := c;
  day := d;
  utilization := ux * 25;
  Utils.pf ch "%s" (Utils.str "(define (problem CrewPlanning_%dcrew_%dday_%dutilization)\n" c d !utilization);
  Utils.pf ch "(:domain CrewPlanning)\n";
  set_parameters ();
  gen_objects ch;
  gen_init ch;
  gen_goal ch;
  Utils.pf ch "(:metric minimize (total-time))\n)\n"




let generate nDay nCrew =
  (** create multiple instance files with the upper limit on Day and
    Crew provided.  for each combination of day/crew we create 3
    instances where the available time utilization is 50%, 75% and 100%
  *)
  Random.self_init ();
  for i = 1 to nDay do
    for j = 1 to nCrew do
      for k = 2 to 4 do
	Wrio.with_outfile (Utils.str "p%d_%d_%d.pddl" i j (k*25))
	  (create_instance i j k)
      done
    done
  done
    


(* EOF *)
