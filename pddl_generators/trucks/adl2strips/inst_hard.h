


/*
 * THIS SOURCE CODE IS SUPPLIED  ``AS IS'' WITHOUT WARRANTY OF ANY KIND, 
 * AND ITS AUTHOR AND THE JOURNAL OF ARTIFICIAL INTELLIGENCE RESEARCH 
 * (JAIR) AND JAIR'S PUBLISHERS AND DISTRIBUTORS, DISCLAIM ANY AND ALL 
 * WARRANTIES, INCLUDING BUT NOT LIMITED TO ANY IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, AND
 * ANY WARRANTIES OR NON INFRINGEMENT.  THE USER ASSUMES ALL LIABILITY AND
 * RESPONSIBILITY FOR USE OF THIS SOURCE CODE, AND NEITHER THE AUTHOR NOR
 * JAIR, NOR JAIR'S PUBLISHERS AND DISTRIBUTORS, WILL BE LIABLE FOR 
 * DAMAGES OF ANY KIND RESULTING FROM ITS USE.  Without limiting the 
 * generality of the foregoing, neither the author, nor JAIR, nor JAIR's
 * publishers and distributors, warrant that the Source Code will be 
 * error-free, will operate without interruption, or will meet the needs 
 * of the user.
 */









/*********************************************************************
 * File: inst_hard.h
 * Description: headers for multiplying hard operators.
 *
 *
 * Author: Joerg Hoffmann 2000
 *
 *********************************************************************/ 








#ifndef _INST_HARD_H
#define _INST_HARD_H



void build_hard_action_templates( void );



void cleanup_hard_domain( void );
Bool var_used_in_literals( int code_var, Literal *ef );
void decrement_inferior_vars_in_literals( int var, Literal *ef );



void multiply_hard_op_parameters( void );
void create_hard_mixed_operators( Operator *o, int curr_var );
Effect *instantiate_Effect( Effect *e );
WffNode *instantiate_wff( WffNode *w );
Bool full_possibly_positive( Fact *f );
Bool full_possibly_negative( Fact *f );
int instantiated_fact_adress( Fact *f );



void multiply_hard_effect_parameters( void );
void create_hard_pseudo_effects( PseudoAction *a, Effect *e, int curr_var );
void make_instantiate_literals( PseudoActionEffect *e, Literal *ll );



#endif /* _INST_HARD_H */
