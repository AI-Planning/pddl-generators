(*
  Tool to generate the test case for packaging machine controller
*)

let get_args () =
  let  nDay = ref 1
   and nCrew = ref 1
   and nUtil = ref 1
   and seed = ref 1 in
    Arg.parse ["-day", Arg.Int (fun i -> nDay := i),
	       "number of day: plan horizon";
	       "-crew", Arg.Int (fun i -> nCrew := i),
	       "number of crew members";
	       "-util", Arg.Int (fun i -> nUtil := i),
	       "utilization level. 2: 50%, 3: 75%, 4:100% ";
               "-seed", Arg.Int (fun i -> seed := i),
	       "random seed";
      ]
      (fun s -> raise (Arg.Bad s))
      "Test generator for OP2, by Minh Do (minh.do@parc.com)\nRevised for modern ocaml system, by Masataro Asai (guicho271828@gmail.com)\n";
    !nDay, !nCrew, !nUtil, !seed



let main () =
  let nDay, nCrew, nUtil, seed = get_args () in
  Random.init (seed);
  if nDay > 50 || nCrew > 50 then
    failwith "Can only support up to 50 CrewMembers and 50 days\n";
  Output.generate nDay nCrew nUtil


let _ = main ()

	  
(* EOF *)
