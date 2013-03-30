(*
  Tool to generate the test case for packaging machine controller
*)

let get_args () =
  let  nDay = ref 1
  and nCrew = ref 1 in
    Arg.parse ["-day", Arg.Int (fun i -> nDay := i),
	       "\tupper limit on number of day: plan horizon";
	       "-crew", Arg.Int (fun i -> nCrew := i),
	       "\tupper limit on the number of crew members"]
      (fun s -> raise (Arg.Bad s))
      "Test generator for OP2, by Minh Do (minh.do@parc.com)";
    !nDay, !nCrew



let main () =
  let nDay, nCrew = get_args () in
    Verb.with_level 3
       (fun () ->
	  if nDay > 50 or nCrew > 50 then
	    failwith "Can only support up to 50 CrewMembers and 50 days\n";
	  Output.generate nDay nCrew)


let _ = main ()

	  
(* EOF *)
