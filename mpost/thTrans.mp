% default translations

let p_station_fixed = p_station_fixed_ASF;
let p_station_painted = p_station_painted_SKBB;
let p_station_natural = p_station_natural_ASF;              
let p_station_temporary = p_station_temporary_ASF;

let p_waterflow_paleo = p_waterflow_paleo_UIS;
let p_waterflow_permanent = p_waterflow_permanent_UIS;
let p_waterflow_intermittent = p_waterflow_intermittent_UIS;

let p_stalactite = p_stalactite_UIS;
let p_stalagmite = p_stalagmite_UIS;
let p_pillar = p_pillar_UIS;
let p_curtain = p_curtain_UIS;
let p_helictite = p_helictite_UIS;
let p_sodastraw = p_sodastraw_UIS;
let p_crystal = p_crystal_UIS;
let p_flowstone = p_flowstone_UIS;
let p_moonmilk = p_moonmilk_UIS;
let p_wallcalcite = p_wallcalcite_UIS;
let p_popcorn = p_popcorn_UIS;
let p_disk = p_disk_UIS;
let p_gypsum = p_gypsum_CCNP;
let p_aragonite = p_aragonite_CCNP;
let p_cavepearl = p_cavepearl_SKBB;
let p_gypsumflower = p_gypsumflower_CCNP;
let p_rimstonepool = p_rimstonepool_ASF;
let p_rimstonedam = p_rimstonedam_ASF;
let p_anastomosis = p_anastomosis_UIS;
let p_karren = p_karren_UIS;
let p_scallop = p_scallop_UIS;
let p_flute = p_flute_UIS;
let p_raft = p_raft_CCNP;
let p_raftcone = p_raftcone_CCNP;
let p_spring = p_spring_SKBB;
let p_sink = p_sink_SKBB;
let p_narrowend = p_narrowend_UIS;
let p_lowend = p_lowend_UIS;
let p_flowstonechoke = p_flowstonechoke_CCNP;
let p_breakdownchoke = p_breakdownchoke_CCNP;
let p_bedrock = p_bedrock_ASF;
let p_clay = p_clay_UIS;                        
let p_sand = p_sand_UIS;
let p_pebbles = p_pebbles_UIS;
let p_debris = p_debris_UIS;
let p_blocks = p_blocks_UIS;
let p_water = p_water_UIS;
let p_ice = p_ice_UIS;
let p_archeomaterial = p_archeomaterial_UIS;
let p_paleomaterial = p_paleomaterial_UIS;
let p_guano = p_guano_UIS;
let p_vegetabledebris = p_vegetabledebris_ASF;
let p_root = p_root_ASF;
let p_entrance = p_entrance_UIS;
let p_gradient = p_gradient_UIS;
let p_rope = p_rope_SKBB;          
let p_fixedladder = p_fixedladder_SKBB;   
let p_ropeladder = p_ropeladder_SKBB;    
let p_steps = p_steps_SKBB;         
let p_bridge = p_bridge_SKBB;        
let p_traverse = p_traverse_SKBB;
let p_anchor = p_anchor_SKBB;
let p_camp = p_camp_SKBB;
let p_noequipment = p_noequipment_SKBB;
let p_sectionarrow = p_sectionarrow_SKBB;
let p_continuation = p_continuation_UIS;
let p_airdraught = p_airdraught_UIS;

let l_wall_bedrock = l_wall_bedrock_UIS;
let l_wall_sand = l_wall_sand_SKBB;
let l_wall_clay = l_wall_clay_SKBB;
let l_wall_pebbles = l_wall_pebbles_SKBB;
let l_wall_debris = l_wall_debris_SKBB;
let l_wall_blocks = l_wall_blocks_SKBB;
let l_wall_ice = l_wall_ice_SKBB;
let l_wall_underlying = l_wall_underlying_UIS;
let l_wall_unsurveyed = l_wall_unsurveyed_SKBB;
let l_wall_presumed = l_wall_presumed_UIS;
%let l_wall_invisible = l_wall_invisible;

let l_waterflow_permanent = l_waterflow_permanent_UIS;
let l_waterflow_intermittent = l_waterflow_intermittent_SKBB;
let l_waterflow_conjectural = l_waterflow_conjectural_SKBB;

let l_border_visible = l_border_visible_SKBB;
let l_border_temporary = l_border_temporary_SKBB;
%let l_border_invisible = l_border_invisible;

let l_floorstep = l_floorstep_UIS;
let l_pit = l_pit_UIS;
let l_ceilingstep = l_ceilingstep_SKBB;
let l_chimney = l_chimney_UIS;
let l_overhang = l_overhang_SKBB;
let l_slope = l_slope_SKBB;
let l_contour = l_contour_UIS;
let l_rockborder = l_rockborder_UIS;
let l_rockedge = l_rockedge_UIS;
let l_flowstone = l_flowstone_UIS;
let l_section = l_section_SKBB;
let l_survey = l_survey_SKBB;
let l_arrow = l_arrow_SKBB;

let PatternFill = thPatternFill;

def a_water = scantokens("a_water_UIS") enddef;
def a_sump = scantokens("a_sump_UIS") enddef;
def a_sand = scantokens("a_sand_UIS") enddef;
def a_debris = scantokens("a_debris_SKBB") enddef;

let s_northarrow = s_northarrow_SKBB;
let s_scalebar = s_scalebar_SKBB;


%def Pebblespattern = scantokens("thDebrisPattern_SKBB") enddef;
%def Claypattern = scantokens("thSandPattern_UIS") enddef;
%def Blockspattern = scantokens("thDebrisPattern_SKBB") enddef;
%def Flowstonepattern = scantokens("a_water_UIS") enddef;
%def Moonmilkpattern = scantokens("a_water_UIS") enddef;
%def Icepattern = scantokens("a_water_UIS") enddef;


%let Boxwork = p_undefined;                      % !
%let SedimentChoke = p_undefined;               % !


%let Floorcanyon = l_undefined;
%let Ceilingcanyon = l_undefined;
%let Rimstonepath = l_undefined;
%let Moonmilkpath = l_undefined;
%let Fixedladderpath = l_undefined;
%let Ropeladderpath = l_undefined;
%let Bridgepath = l_undefined;
%let Fixedline = l_undefined;
%let Handrail = l_undefined;
%let Fence = l_undefined;
%let Concretewall = l_undefined;
%let Door = l_undefined;
%let Gate = l_undefined;
%let Formedpath = l_undefined;
%let Stepspath = l_undefined;
%let Bridgepath = l_undefined;
