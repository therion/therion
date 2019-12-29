% Symbol set of Brazilian Speleological Society (SBE - Sociedade Brasileira de 
% Espeleologia), author Rodrigo Severo.


%%%%%%%%%%%%%%%%%%%%%%%%% Points %%%%%%%%%%%%%%%%%%%%%%%%%


picture SBE_altar_raw;
SBE_altar_raw := image(
	begingroup;
		fill (406,482)..controls (364,491) and (340,542)..(360,581)
			..controls (378,621) and (436,633)..(469,603)
			..controls (504,575) and (500,514)..(461,491)
			..controls (445,480) and (425,478)..(406,482)
			--cycle;
		pickup pencircle scaled 99bp;
		draw (225,307)--(319,460);
		pickup pencircle scaled 70bp;
		draw (329,472)--(391,364)--(466,419);
		draw (154,188)--(326,188)--(224,290);
	endgroup;
);

def p_altar_SBE(expr pos, theta, sc, al) = 
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_altar_raw shifted (-(llcorner SBE_altar_raw)-(urcorner SBE_altar_raw - llcorner SBE_altar_raw)/2) scaled (u / max((xpart urcorner SBE_altar_raw) - (xpart llcorner SBE_altar_raw), (ypart urcorner SBE_altar_raw) - (ypart llcorner SBE_altar_raw)));
enddef;


picture SBE_archeo_excavation_raw;
SBE_archeo_excavation_raw := image(
	begingroup;
		interim linecap := butt;
		interim linejoin := mitered;
		fill (199,415)--(242,555)--(312,574)
			--(329,558)--(351,580)--(335,597)
			--(422,683)--(466,640)--(379,553)
			--(362,569)--(340,547)--(359,528)
			--(339,457)--cycle;
		pickup pencircle scaled 17bp;
		interim linecap := rounded;
		draw (138,464)..controls (152,460) and (169,461)..(187,461);
		draw (368,463)..controls (411,458) and (449,461)..(489,464);
		pickup pencircle scaled 11bp;
		interim linecap := squared;
		draw (291,403)..controls (279,402) and (262,395)..(253,372);
		draw (371,409)..controls (370,422) and (359,429)..(346,430);
		draw (423,399)..controls (432,402) and (437,411)..(437,418);
	endgroup;
);

def p_archeoexcavation_SBE(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_archeo_excavation_raw shifted (-(llcorner SBE_archeo_excavation_raw)-(urcorner SBE_archeo_excavation_raw - llcorner SBE_archeo_excavation_raw)/2) scaled (u / max((xpart urcorner SBE_archeo_excavation_raw) - (xpart llcorner SBE_archeo_excavation_raw), (ypart urcorner SBE_archeo_excavation_raw) - (ypart llcorner SBE_archeo_excavation_raw)));
enddef;


picture SBE_audio_raw;
SBE_audio_raw := image(
	begingroup;
		interim linecap := butt;
		interim linejoin := mitered;
		fill (125,196)--(81,171)--(44,171)
			..controls (34,171) and (27,163)..(27,154)
			--(27,115)..controls (27,106) and (34,98)..(44,98)
			--(81,98)--(125,73)..controls (133,68) and (140,72)..(140,81)
			--(140,188)..controls (140,197) and (133,201)..(125,196)
			--cycle;
		pickup pencircle scaled 11bp;
		draw (158,186)..controls (178,152) and (179,118)..(158,82);
		draw (185,202)..controls (208,166) and (213,112)..(185,67);
		draw (213,218)..controls (248,150) and (243,105)..(213,50);
	endgroup;
);

def p_audio_SBE(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_audio_raw shifted (-(llcorner SBE_audio_raw)-(urcorner SBE_audio_raw - llcorner SBE_audio_raw)/2) scaled (u / max((xpart urcorner SBE_audio_raw) - (xpart llcorner SBE_audio_raw), (ypart urcorner SBE_audio_raw) - (ypart llcorner SBE_audio_raw)));
enddef;


picture SBE_bat_raw;
SBE_bat_raw := image(
	begingroup;
		interim linecap := butt;
		interim linejoin := mitered;
		fill (538,34)..controls (484,144) and (350,326)..(287,197)
			..controls (249,375) and (141,347)..(83,352)
			..controls (58,356) and (10,339)..(53,371)
			..controls (100,409) and (147,451)..(204,474)
			..controls (286,503) and (318,471)..(376,407)
			..controls (439,342) and (510,278)..(599,252)
			..controls (601,278) and (580,480)..(623,338)
			..controls (621,373) and (696,393)..(707,345)
			..controls (719,377) and (741,417)..(737,370)
			..controls (732,310) and (745,281)..(749,252)
			..controls (837,323) and (902,458)..(1017,478)
			..controls (1114,499) and (1201,436)..(1266,371)
			..controls (1285,353) and (1316,330)..(1270,343)
			..controls (1174,357) and (1033,396)..(1018,231)
			..controls (905,326) and (818,126)..(798,41)
			..controls (718,124) and (662,-13)..(637,34)
			..controls (621,76) and (562,52)..(538,34)
			--cycle;
	endgroup;
);

def p_bat_SBE(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_bat_raw shifted (-(llcorner SBE_bat_raw)-(urcorner SBE_bat_raw - llcorner SBE_bat_raw)/2) scaled (u / max((xpart urcorner SBE_bat_raw) - (xpart llcorner SBE_bat_raw), (ypart urcorner SBE_bat_raw) - (ypart llcorner SBE_bat_raw)));
enddef;


picture SBE_bones_raw;
SBE_bones_raw := image(
	begingroup;
		interim linecap := butt;
		interim linejoin := mitered;
		fill (328,272)..controls (258,226) and (188,192)..(132,134)
			..controls (119,115) and (103,80)..(80,78)
			..controls (68,76) and (49,83)..(39,93)
			..controls (35,97) and (15,102)..(25,118)
			..controls (44,150) and (30,150)..(18,154)
			..controls (6,159) and (-2,159)..(-3,168)
			..controls (2,202) and (32,206)..(54,205)
			..controls (83,204) and (94,199)..(126,215)
			..controls (148,226) and (183,243)..(278,300)
			--cycle;
		fill (663,585)..controls (648,585) and (632,579)..(620,571)
			..controls (609,563) and (597,547)..(587,527)
			..controls (571,497) and (555,479)..(529,463)
			..controls (506,448) and (493,443)..(445,412)
			--(501,378)..controls (544,403) and (605,415)..(650,416)
			..controls (671,416) and (672,416)..(683,422)
			..controls (708,436) and (714,455)..(701,474)
			..controls (674,489) and (653,488)..(674,511)
			..controls (686,524) and (702,548)..(698,562)
			..controls (692,578) and (679,585)..(663,585)
			--cycle;
		fill (584,155)..controls (572,180) and (570,192)..(541,213)
			..controls (506,237) and (222,404)..(158,436)
			..controls (118,456) and (92,463)..(43,466)
			..controls (22,468) and (21,468)..(10,474)
			..controls (-12,490) and (-17,510)..(-3,527)
			..controls (23,541) and (45,538)..(25,563)
			..controls (15,576) and (0,601)..(5,615)
			..controls (19,647) and (61,636)..(83,619)
			..controls (93,610) and (104,593)..(114,572)
			..controls (127,542) and (142,523)..(166,505)
			..controls (205,477) and (361,387)..(421,358)
			..controls (480,330) and (597,259)..(609,256)
			..controls (631,255) and (670,257)..(683,238)
			..controls (690,228) and (693,208)..(690,195)
			..controls (688,189) and (693,170)..(674,171)
			..controls (637,172) and (644,159)..(646,147)
			..controls (648,134) and (652,126)..(644,121)
			..controls (612,110) and (594,134)..(584,155)
			--cycle;
	endgroup;
);

def p_bones_SBE(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_bones_raw shifted (-(llcorner SBE_bones_raw)-(urcorner SBE_bones_raw - llcorner SBE_bones_raw)/2) scaled (u / max((xpart urcorner SBE_bones_raw) - (xpart llcorner SBE_bones_raw), (ypart urcorner SBE_bones_raw) - (ypart llcorner SBE_bones_raw)));
enddef;


def p_curtains_SBE(expr pos, theta, sc, al) =
	U:=(.3u, .8u);
	pickup PenC;
	for i=-1 upto 1:
		T:=identity rotated theta aligned al scaled (sc * 0.7) shifted (pos + (i * .3u, abs(i) * .15u));
		thdraw (-.15u,.4u)--(0,.15u)--(.15u,.4u);
		thdraw (0,.15u)--(0,0){dir 180}..(-.12u,-.11u)..{dir 0}(0,-.22u)--(0,-.4u);
	endfor;
enddef;


picture SBE_danger_raw;
SBE_danger_raw := image(
	begingroup;
		interim linecap := butt;
		interim linejoin := mitered;
		fill (331,489)..controls (330,489) and (328,489)..(326,488)
			--(291,422)..controls (291,422) and (291,421)..(291,421)
			..controls (291,417) and (294,414)..(297,413)
			--(365,413)..controls (369,414) and (371,417)..(371,421)
			..controls (371,422) and (371,422)..(371,423)
			--(336,488)..controls (335,489) and (333,489)..(331,489)
			..controls (331,489) and (331,489)..(331,489)
			--cycle withcolor red;
		fill (336,427)..controls (336,430) and (334,432)..(331,432)
			..controls (328,432) and (326,430)..(326,427)
			..controls (326,424) and (328,422)..(331,422)
			..controls (334,422) and (336,424)..(336,427)
			--cycle withcolor white;
		fill (335,464)..controls (336,466) and (332,466)..(331,466)
			..controls (330,466) and (327,466)..(327,464)
			--(330,436)..controls (330,435) and (330,435)..(331,435)
			..controls (332,435) and (332,435)..(332,436)
			--cycle withcolor white;
	endgroup;
);

def p_danger_SBE(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_danger_raw shifted (-(llcorner SBE_danger_raw)-(urcorner SBE_danger_raw - llcorner SBE_danger_raw)/2) scaled (u / max((xpart urcorner SBE_danger_raw) - (xpart llcorner SBE_danger_raw), (ypart urcorner SBE_danger_raw) - (ypart llcorner SBE_danger_raw)));
enddef;


def p_discpillar_UIS(expr pos,theta,sc,al) =
	U:=(.15u, .4u);
	T:=identity rotated theta aligned al scaled sc shifted pos;
	pickup PenC;
	thdraw (-.15u,.4u) -- (0,.15u) -- (0,-.15u) -- (-.15u,-.4u);
	thdraw (0,-.15u) -- (.15u,-.4u);
	thdraw (0,.15u) -- (.15u,.4u);
	thdraw (-.15u,0) -- (.15u,0);
enddef;


def p_discpillars_UIS(expr pos,theta,sc,al) =
	U:=(.3u, .8u);
	pickup PenC;
	for i=-1 upto 1:
		T:=identity rotated theta aligned al scaled (sc * 0.7) shifted (pos + (i * .3u, abs(i) * .15u));
		thdraw (-.15u,.4u) -- (0,.15u) -- (0,-.15u) -- (-.15u,-.4u);
		thdraw (0,-.15u) -- (.15u,-.4u);
		thdraw (0,.15u) -- (.15u,.4u);
		thdraw (-.15u,0) -- (.15u,0);
	endfor;
enddef;


def p_discstalactites_UIS(expr pos,theta,sc,al) =
	U:=(.3u, .8u);
	pickup PenC;
	for i=-1 upto 1:
		T:=identity rotated theta aligned al scaled (sc * 0.7) shifted (pos + (i * .3u, abs(i) * .15u));
		thdraw (0,-.4u) -- (0,.15u) -- (-.15u,.4u);
		thdraw (0,.15u) -- (.15u,.4u);
		thdraw (-.15u,-.1u) -- (.15u,-.1u);
	endfor;
enddef;


def p_discstalagmites_UIS(expr pos,theta,sc,al) =
	U:=(.3u, .8u);
	pickup PenC;
	for i=-1 upto 1:
		T:=identity rotated theta aligned al scaled (sc * 0.7) shifted (pos + (i * .3u, abs(i) * .15u));
		thdraw (0,.4u) -- (0,-.15u) -- (-.15u,-.4u);
		thdraw (0,-.15u) -- (.15u,-.4u);
		thdraw (-.15u,.1u) -- (.15u,.1u);
	endfor;
enddef;


def p_discstalactite_UIS(expr pos,theta,sc,al) =
	U:=(.15u, .4u);
	T:=identity rotated theta aligned al scaled sc shifted pos;
	pickup PenC;
	thdraw (0,-.4u) -- (0,.15u) -- (-.15u,.4u);
	thdraw (0,.15u) -- (.15u,.4u);
	thdraw (-.15u,-.1u) -- (.15u,-.1u);
enddef;


def p_discstalagmite_UIS(expr pos,theta,sc,al) =
	U:=(.15u, .4u);
	T:=identity rotated theta aligned al scaled sc shifted pos;
	pickup PenC;
	thdraw (0,.4u) -- (0,-.15u) -- (-.15u,-.4u);
	thdraw (0,-.15u) -- (.15u,-.4u);
	thdraw (-.15u,.1u) -- (.15u,.1u);
enddef;


picture SBE_electric_light_raw;
SBE_electric_light_raw := image(
	begingroup;
		interim linecap := butt;
		interim linejoin := mitered;
		fill (334,564)..controls (341,561) and (346,556)..(349,550)
			..controls (350,548) and (351,546)..(351,543)
			..controls (349,543) and (347,543)..(345,543)
			..controls (329,543) and (313,543)..(296,543)
			..controls (294,545) and (296,548)..(297,550)
			..controls (300,556) and (305,561)..(312,564)
			..controls (317,567) and (324,567)..(330,565)
			..controls (332,565) and (333,565)..(334,564)
			--cycle;
		fill (394,393)..controls (402,379) and (406,361)..(404,344)
			..controls (399,312) and (375,287)..(348,280)
			..controls (317,271) and (282,283)..(263,310)
			..controls (246,333) and (244,370)..(258,391)
			..controls (277,419) and (281,466)..(281,466)
			--(368,466)..controls (369,439) and (383,413)..(394,393)
			--cycle;
		pickup pencircle scaled 25bp;
		draw (153,248)--(224,303);
		draw (489,255)--(424,301);
		draw (408,208)--(377,259);
		draw (241,218)--(266,265);
		draw (324,161)--(324,245);
		pickup pencircle scaled 22bp;
		interim linecap := rounded;
		draw (282,489)--(370,489);
		draw (280,524)--(371,524);
	endgroup;
);

def p_electriclight_SBE(expr pos, theta, sc, al) = 
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_electric_light_raw shifted (-(llcorner SBE_electric_light_raw)-(urcorner SBE_electric_light_raw - llcorner SBE_electric_light_raw)/2) scaled (u / max((xpart urcorner SBE_electric_light_raw) - (xpart llcorner SBE_electric_light_raw), (ypart urcorner SBE_electric_light_raw) - (ypart llcorner SBE_electric_light_raw)));
enddef;


picture SBE_exvoto_raw;
SBE_exvoto_raw := image(
	begingroup;
		pickup pencircle scaled 29bp;
		draw (274,659)--(381,623);
		draw (209,277)--(247,393);
		draw (187,282)--(228,270);
		draw (297,647)--(224,427)--(247,393)
			--(283,408)--(357,631);
		draw (251,497)--(302,494);
		draw (490,657)--(578,587);
		draw (297,321)--(373,417);
		draw (282,334)--(313,309);
		draw (508,638)--(363,457)--(373,417)
			--(412,418)--(558,602);
		draw (421,524)--(452,482);
	endgroup;
);

def p_exvoto_SBE(expr pos, theta, sc, al) = 
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_exvoto_raw shifted (-(llcorner SBE_exvoto_raw)-(urcorner SBE_exvoto_raw - llcorner SBE_exvoto_raw)/2) scaled (u / max((xpart urcorner SBE_exvoto_raw) - (xpart llcorner SBE_exvoto_raw), (ypart urcorner SBE_exvoto_raw) - (ypart llcorner SBE_exvoto_raw)));
enddef;


picture SBE_gate_raw;
SBE_gate_raw := image(
	begingroup;
		pickup pencircle scaled 28bp;
		interim linecap := butt;
		interim linejoin := rounded;
		draw (121,221)--(249,244)--(249,546)
			--(121,454)--cycle;
		draw (466,221)--(337,244)--(337,546)
			--(465,454)--cycle;
		pickup pencircle scaled 22bp;
		interim linejoin := mitered;
		draw (121,307)--(249,330);
		draw (184,496)--(184,234);
		draw (466,307)--(337,330);
		draw (402,496)--(402,234);
	endgroup;
);

def p_gate_SBE(expr pos, theta, sc, al) = 
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_gate_raw shifted (-(llcorner SBE_gate_raw)-(urcorner SBE_gate_raw - llcorner SBE_gate_raw)/2) scaled (u / max((xpart urcorner SBE_gate_raw) - (xpart llcorner SBE_gate_raw), (ypart urcorner SBE_gate_raw) - (ypart llcorner SBE_gate_raw)));
enddef;


def p_helictites_UIS(expr pos,theta,sc,al) =
	U:=(.4u,.8u);
	pickup PenC;
	for i=-1 upto 1:
		if i < 0:
			T:=identity rotated theta aligned al scaled (sc * 0.7) shifted (pos + (i * .45u, abs(i) * .15u));
		else:
			T:=identity rotated theta aligned al scaled (sc * 0.7) shifted (pos + (i * .3u, abs(i) * .15u));
		fi;
		thdraw (0,.4u)--(0,-.4u);
		p := (-.2u,.4u)---(-.2u,.1u){down}..{dir -10}(-.1u,0.02u)---origin;
		thdraw p;
		thdraw p reflectedabout (origin,(0,u)) reflectedabout (origin,(u,0));
	endfor;
enddef;


picture SBE_human_bones_raw;
SBE_human_bones_raw := image(
	begingroup;
		interim linecap := butt;
		interim linejoin := mitered;
		linecap := butt;
		linejoin := mitered;
		fill (52,279)..controls (50,279) and (48,279)..(47,278)
			--(47,278)..controls (37,276) and (32,266)..(33,258)
			..controls (26,254) and (20,247)..(20,240)
			..controls (20,230) and (30,221)..(41,221)
			..controls (45,222) and (49,223)..(52,225)
			--(88,205)..controls (88,205) and (88,206)..(87,206)
			--(86,210)..controls (82,221) and (81,236)..(82,251)
			--(72,257)..controls (72,261) and (71,265)..(70,268)
			..controls (66,275) and (59,279)..(52,279)
			--cycle;
		fill (258,151)..controls (256,139) and (255,125)..(249,113)
			--(296,87)..controls (295,79) and (300,71)..(306,67)
			..controls (323,63) and (335,72)..(335,86)
			..controls (342,90) and (347,97)..(347,104)
			..controls (347,114) and (337,123)..(326,122)
			..controls (322,122) and (319,121)..(315,119)
			--cycle;
		fill (316,279)..controls (309,279) and (302,275)..(298,268)
			..controls (296,265) and (296,261)..(296,257)
			--(291,254)..controls (293,239) and (292,221)..(288,210)
			--(288,209)--(315,225)..controls (319,223) and (322,222)..(327,221)
			..controls (338,221) and (347,230)..(347,240)
			..controls (347,247) and (342,254)..(335,258)
			..controls (335,266) and (331,276)..(321,278)
			--(321,278)..controls (319,279) and (318,279)..(316,279)
			--cycle;
		fill (116,155)--(52,119)..controls (49,121) and (46,122)..(41,122)
			..controls (30,123) and (20,114)..(20,104)
			..controls (20,97) and (25,90)..(33,86)
			..controls (32,72) and (44,63)..(62,67)
			..controls (67,71) and (72,79)..(72,87)
			--(123,116)..controls (118,128) and (117,143)..(116,155)
			--cycle;
		fill (187,311)..controls (179,311) and (171,310)..(165,308)
			..controls (136,296) and (120,285)..(109,255)
			..controls (105,242) and (105,223)..(109,211)
			--(110,208)..controls (119,188) and (127,188)..(129,184)
			..controls (135,174) and (130,139)..(148,126)
			..controls (156,124) and (165,119)..(171,120)
			..controls (176,122) and (182,120)..(187,120)
			..controls (192,120) and (198,121)..(203,120)
			..controls (211,120) and (218,123)..(226,126)
			..controls (244,139) and (239,174)..(245,184)
			..controls (246,188) and (255,188)..(263,208)
			--(265,211)..controls (269,223) and (269,242)..(265,255)
			..controls (265,255) and (259,272)..(249,282)
			..controls (236,295) and (230,302)..(209,308)
			..controls (203,310) and (195,311)..(187,311)
			--cycle;
		fill (171,230)..controls (162,217) and (161,207)..(148,207)
			..controls (125,206) and (126,224)..(125,231)
			..controls (123,240) and (136,248)..(148,248)
			..controls (159,248) and (176,239)..(171,230)
			--cycle withcolor white;
		fill (203,230)..controls (212,217) and (212,207)..(226,207)
			..controls (249,206) and (247,224)..(249,231)
			..controls (251,240) and (238,248)..(226,248)
			..controls (215,248) and (198,239)..(203,230)
			--cycle withcolor white;
		fill (187,212)..controls (177,207) and (175,183)..(175,174)
			..controls (175,174) and (176,173)..(177,173)
			..controls (186,178) and (189,178)..(198,173)
			..controls (199,173) and (200,174)..(200,174)
			..controls (200,183) and (197,213)..(187,212)
			--cycle withcolor white;
	endgroup;
);

def p_humanbones_SBE(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_human_bones_raw shifted (-(llcorner SBE_human_bones_raw)-(urcorner SBE_human_bones_raw - llcorner SBE_human_bones_raw)/2) scaled (u / max((xpart urcorner SBE_human_bones_raw) - (xpart llcorner SBE_human_bones_raw), (ypart urcorner SBE_human_bones_raw) - (ypart llcorner SBE_human_bones_raw)));
enddef;


picture SBE_masonry_raw;
SBE_masonry_raw := image(
	begingroup;
		pickup pencircle scaled 28bp;
		interim linecap := butt;
		interim linejoin := mitered;
		draw (287,482)--(355,482);
		draw (247,442)--(315,442);
		draw (327,442)--(395,442);
		draw (206,401)--(275,401);
		draw (287,401)--(355,401);
		draw (247,360)--(315,360);
		draw (327,360)--(395,360);
	endgroup;
);

def p_masonry_SBE(expr pos, theta, sc, al) = 
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_masonry_raw shifted (-(llcorner SBE_masonry_raw)-(urcorner SBE_masonry_raw - llcorner SBE_masonry_raw)/2) scaled (u / max((xpart urcorner SBE_masonry_raw) - (xpart llcorner SBE_masonry_raw), (ypart urcorner SBE_masonry_raw) - (ypart llcorner SBE_masonry_raw)));
enddef;


picture UIS_minus_raw;
UIS_minus_raw := image(
	fill (294.660004,411.578003)--(307.644989,411.578003)--(307.644989,407.625000)
		--(294.660004,407.625000)--cycle;
	pickup pencircle scaled 1.285130bp;
	draw (314.710999,409.601990)..controls (314.710999,402.113007) and (308.640991,396.042999)..(301.152008,396.042999)
		..controls (293.664001,396.042999) and (287.593994,402.113007)..(287.593994,409.601990)
		..controls (287.593994,417.089996) and (293.664001,423.160004)..(301.152008,423.160004)
		..controls (308.640991,423.160004) and (314.710999,417.089996)..(314.710999,409.601990)
		--cycle withcolor black;
);
picture UIS_minus;
UIS_minus := UIS_minus_raw shifted (-(llcorner UIS_minus_raw)-(urcorner UIS_minus_raw - llcorner UIS_minus_raw)/2) scaled (u / max((xpart urcorner UIS_minus_raw) - (xpart llcorner UIS_minus_raw), (ypart urcorner UIS_minus_raw) - (ypart llcorner UIS_minus_raw)));

def p_minus_UIS(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw UIS_minus;
enddef;


picture SBE_mud_raw;
SBE_mud_raw := image(
	begingroup;
		interim linecap := butt;
		interim linejoin := mitered;
		fill (240,321)..controls (232,352) and (217,388)..(183,397)
			..controls (156,398) and (135,369)..(108,381)
			..controls (82,388) and (66,417)..(74,442)
			..controls (78,479) and (59,518)..(79,552)
			..controls (87,582) and (41,598)..(52,628)
			..controls (68,653) and (99,634)..(121,630)
			..controls (147,623) and (162,645)..(172,664)
			..controls (200,685) and (236,674)..(267,676)
			..controls (290,675) and (312,676)..(333,685)
			..controls (352,692) and (380,696)..(394,676)
			..controls (406,653) and (421,627)..(450,623)
			..controls (474,620) and (506,635)..(520,605)
			..controls (536,576) and (533,543)..(533,512)
			..controls (531,476) and (511,442)..(521,407)
			..controls (523,374) and (483,370)..(459,370)
			..controls (431,371) and (407,359)..(382,348)
			..controls (357,337) and (326,324)..(300,340)
			..controls (277,343) and (264,298)..(241,320)
			--(240,320)--cycle withcolor (0.53, 0.24, 0.02);
	endgroup;
);

def p_mud_SBE(expr pos, theta, sc, al) = 
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_mud_raw shifted (-(llcorner SBE_mud_raw)-(urcorner SBE_mud_raw - llcorner SBE_mud_raw)/2) scaled (u / max((xpart urcorner SBE_mud_raw) - (xpart llcorner SBE_mud_raw), (ypart urcorner SBE_mud_raw) - (ypart llcorner SBE_mud_raw)));
enddef;


picture SBE_mud_crack_raw;
SBE_mud_crack_raw := image(
	begingroup;
		interim linecap := butt;
		interim linejoin := mitered;
		fill (320,501)--(340,468)--(320,462)
			--(320,462)--(320,457)--(348,464)
			--(387,451)--(371,434)--(345,421)
			--(320,427)--(320,427)--(320,424)
			--(331,422)--(320,385)--(325,385)
			--(335,422)--(344,418)--(351,385)
			--(355,385)--(355,385)--(348,419)
			--(373,429)--(401,403)--(393,388)
			--(395,385)--(397,385)--(396,387)
			--(403,402)--(416,400)--(428,389)
			--(429,390)--(417,404)--(403,404)
			--(378,432)--(393,448)--(431,415)
			--(436,415)--(436,419)--(433,419)
			--(396,452)--(406,464)--(436,460)
			--(436,464)--(408,468)--(403,484)
			--(436,490)--(436,493)--(403,488)
			--(403,501)--(399,501)--(400,483)
			--(403,468)--(391,457)--(351,467)
			--(378,501)--(370,501)--(346,468)
			--(324,501)--cycle;
	endgroup;
);

def p_mudcrack_SBE(expr pos, theta, sc, al) =
    U:=(.5u,.5u);
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_mud_crack_raw shifted (-(llcorner SBE_mud_crack_raw)-(urcorner SBE_mud_crack_raw - llcorner SBE_mud_crack_raw)/2) scaled (u / max((xpart urcorner SBE_mud_crack_raw) - (xpart llcorner SBE_mud_crack_raw), (ypart urcorner SBE_mud_crack_raw) - (ypart llcorner SBE_mud_crack_raw)));
enddef;


picture SBE_nameplate_raw;
SBE_nameplate_raw := image(
	pickup pencircle scaled 24bp;
	begingroup;
		interim linecap := butt;
		interim linejoin := mitered;
		draw (77,563)--(505,563)--(505,340)
			--(77,340)--cycle;
		pickup pencircle scaled 10bp;
		draw (208,496)--(422,496);
		draw (117,450)--(358,450);
		draw (264,401)--(470,401);
	endgroup;
);

def p_nameplate_SBE(expr pos, theta, sc, al) = 
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_nameplate_raw shifted (-(llcorner SBE_nameplate_raw)-(urcorner SBE_nameplate_raw - llcorner SBE_nameplate_raw)/2) scaled (u / max((xpart urcorner SBE_nameplate_raw) - (xpart llcorner SBE_nameplate_raw), (ypart urcorner SBE_nameplate_raw) - (ypart llcorner SBE_nameplate_raw)));
enddef;


picture SBE_notwheelchair_raw;
SBE_notwheelchair_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (591.965027,447.699005)..controls (591.965027,295.476990) and (465.324005,172.074005)..(309.105011,172.074005)
		..controls (152.891006,172.074005) and (26.250000,295.476990)..(26.250000,447.699005)
		..controls (26.250000,599.921997) and (152.891006,723.323975)..(309.105011,723.323975)
		..controls (465.324005,723.323975) and (591.965027,599.921997)..(591.965027,447.699005)
		--cycle withcolor white;
	fill (260.250000,282.113007)..controls (248.195007,283.492004) and (243.151993,284.300995)..(236.863007,285.871002)
		..controls (185.647995,298.648010) and (145.852005,340.722992)..(137.608994,390.808990)
		..controls (128.695007,444.988007) and (155.022995,497.621002)..(204.750000,525.034973)
		..controls (218.397995,532.554993) and (225.742004,534.898010)..(231.688004,533.625000)
		..controls (240.770004,531.676025) and (246.276993,523.551025)..(244.414001,514.851990)
		..controls (243.272995,509.546997) and (239.714996,505.906006)..(232.733994,502.910004)
		..controls (203.906006,490.539001) and (183.820007,469.671997)..(174.104996,442.011993)
		..controls (170.434006,431.566010) and (169.188004,423.738007)..(169.223007,411.433990)
		..controls (169.276993,394.613007) and (172.070007,382.855011)..(179.453003,368.398010)
		..controls (193.070007,341.734009) and (218.542999,322.542999)..(249.375000,315.722992)
		..controls (257.585999,313.906006) and (277.851990,313.507996)..(286.500000,314.992004)
		..controls (326.503998,321.859009) and (357.753998,349.492004)..(368.660004,387.632996)
		..controls (370.539001,394.207001) and (372.628998,397.679993)..(376.156006,400.097992)
		..controls (381.507996,403.765991) and (387.429993,403.855011)..(393.906006,400.375000)
		..controls (402.406006,395.808990) and (403.757996,388.308990)..(398.738007,373.558990)
		..controls (395.742004,364.742004) and (389.175995,350.457001)..(385.296997,344.308990)
		..controls (365.308990,312.625000) and (332.300995,290.769989)..(294.000000,283.851990)
		..controls (286.496002,282.496002) and (266.089996,281.445007)..(260.250000,282.113007)
		--cycle;
	fill (467.046997,312.886993)..controls (460.328003,314.953003) and (459.062988,316.832001)..(429.351990,368.847992)
		--(401.625000,417.390991)--(350.574005,417.410004)..controls (294.496002,417.433990) and (295.515991,417.359009)..(288.488007,422.011993)
		..controls (283.910004,425.042999) and (280.011993,430.167999)..(278.609009,435.000000)
		..controls (276.437988,442.480011) and (255.000000,547.281006)..(255.000000,550.418030)
		..controls (255.000000,561.828003) and (262.167999,572.448975)..(273.425995,577.723022)
		..controls (277.445007,579.604980) and (278.671997,579.804993)..(286.125000,579.804993)
		..controls (293.894989,579.804993) and (294.664001,579.664001)..(299.304993,577.395020)
		..controls (305.285004,574.468994) and (311.304993,568.382996)..(313.964996,562.573975)
		..controls (314.996002,560.312988) and (316.761993,553.648010)..(317.878998,547.762024)
		--(319.914001,537.054993)--(388.125000,536.304993)--(391.703003,534.203003)
		..controls (396.632996,531.304993) and (399.203003,527.210999)..(399.609009,521.612976)
		..controls (400.128998,514.421997) and (397.351990,509.363007)..(391.207001,506.300995)
		--(387.457001,504.429993)--(326.972992,504.429993)--(327.367004,502.742004)
		..controls (327.582001,501.816010) and (329.292999,493.460999)..(331.167999,484.179993)
		..controls (333.039001,474.898010) and (334.757996,466.558990)..(334.984009,465.644989)
		--(335.398010,463.984009)--(373.781006,463.769989)--(412.160004,463.554993)
		--(417.015991,461.164001)..controls (419.687988,459.847992) and (422.953003,457.652008)..(424.273010,456.289001)
		..controls (427.167999,453.292999) and (490.937988,342.886993)..(492.855011,337.554993)
		..controls (495.515991,330.144989) and (493.949005,323.308990)..(488.285004,317.644989)
		..controls (482.695007,312.054993) and (475.207001,310.378998)..(467.046997,312.886993)
		--cycle;
	fill (265.523010,590.382996)..controls (252.574005,593.862976) and (241.207001,605.551025)..(238.147995,618.534973)
		..controls (235.007996,631.851990) and (238.602005,644.034973)..(248.625000,654.059021)
		..controls (257.054993,662.492004) and (267.546997,666.476990)..(278.589996,665.445007)
		..controls (291.546997,664.237976) and (304.082001,655.718994)..(309.386993,644.520020)
		..controls (316.609009,629.265991) and (313.773010,612.031006)..(302.121002,600.395020)
		..controls (299.457001,597.734009) and (295.539001,594.612976)..(293.414001,593.460999)
		..controls (286.046997,589.465027) and (273.917999,588.125000)..(265.523010,590.382996)
		--cycle;
	fill (481.250000,197.550995)..controls (482.925995,198.742004) and (488.730011,203.207001)..(494.148010,207.477005)
		..controls (585.690979,279.574005) and (628.534973,396.152008)..(605.468994,510.375000)
		..controls (598.546997,544.637024) and (587.534973,573.164001)..(569.078003,604.640991)
		..controls (551.426025,634.745972) and (533.632996,656.590027)..(508.593994,678.882996)
		..controls (426.164001,752.270020) and (312.156006,775.796997)..(207.011993,741.112976)
		..controls (184.641006,733.734009) and (175.645004,729.515991)..(149.039001,713.929993)
		..controls (122.433998,698.348022) and (114.351997,692.562988)..(96.976601,676.659973)
		..controls (9.855470,596.937988) and (-24.203100,475.480011)..(8.941410,362.707001)
		..controls (16.246099,337.839996) and (24.953100,317.933990)..(39.003899,293.945007)
		..controls (57.386700,262.554993) and (76.851601,239.078003)..(103.535004,216.102005)
		..controls (184.238007,146.617004) and (294.851990,123.917999)..(396.695007,155.945007)
		..controls (419.683990,163.171997) and (430.769989,168.151993)..(455.578003,182.395004)
		..controls (468.019989,189.539001) and (479.570007,196.358994)..(481.250000,197.550995)
		--cycle--(277.601990,213.636993)..controls (233.565994,218.679993) and (193.539001,235.098007)..(157.531006,262.871002)
		..controls (147.410004,270.679993) and (125.598000,292.492004)..(117.789001,302.613007)
		..controls (84.257797,346.082001) and (67.269501,395.371002)..(67.273399,449.175995)
		..controls (67.273399,487.957001) and (75.441399,522.137024)..(92.996101,556.831970)
		..controls (98.546898,567.809021) and (111.301003,588.296997)..(112.578003,588.296997)
		..controls (113.430000,588.296997) and (443.218994,258.511993)..(443.218994,257.660004)
		..controls (443.218994,256.382996) and (422.730011,243.632996)..(411.750000,238.078003)
		..controls (385.773010,224.934006) and (359.667999,216.996002)..(331.425995,213.656006)
		..controls (318.093994,212.078003) and (291.273010,212.070007)..(277.601990,213.632996)
		--cycle--(328.210999,473.515991)--(162.276993,639.448975)--(173.772995,647.090027)
		..controls (195.973007,661.848022) and (226.550995,674.929993)..(251.934006,680.520020)
		..controls (271.328003,684.793030) and (279.609009,685.585999)..(304.515991,685.559021)
		..controls (330.675995,685.526978) and (342.062988,684.237976)..(362.359009,679.000000)
		..controls (387.902008,672.409973) and (413.476990,661.065979)..(434.562988,646.968994)
		..controls (449.218994,637.168030) and (456.722992,631.085999)..(469.609009,618.551025)
		..controls (506.695007,582.473022) and (530.120972,537.034973)..(538.559021,484.800995)
		..controls (541.000000,469.695007) and (541.581970,439.566010)..(539.745972,423.511993)
		..controls (535.359009,385.167999) and (522.723022,350.160004)..(501.964996,318.828003)
		..controls (497.867004,312.640991) and (494.429993,307.582001)..(494.328003,307.582001)
		..controls (494.226990,307.582001) and (419.472992,382.250000)..(328.210999,473.515991)
		--cycle withcolor red;
	endgroup;
);
picture SBE_notwheelchair;
SBE_notwheelchair := SBE_notwheelchair_raw shifted (-(llcorner SBE_notwheelchair_raw)-(urcorner SBE_notwheelchair_raw - llcorner SBE_notwheelchair_raw)/2) scaled (u / max((xpart urcorner SBE_notwheelchair_raw) - (xpart llcorner SBE_notwheelchair_raw), (ypart urcorner SBE_notwheelchair_raw) - (ypart llcorner SBE_notwheelchair_raw)));

def p_nowheelchair_SBE(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_notwheelchair;
enddef;


picture SBE_pendant_raw;
SBE_pendant_raw := image(
	pickup pencircle scaled 22.011000bp;
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	draw (476.980011,639.585999)..controls (471.453003,639.585999) and (422.562988,621.965027)..(428.812988,575.375000)
		..controls (434.457001,533.296997) and (506.785004,503.894989)..(505.894989,457.292999)
		..controls (502.128998,260.425995) and (383.000000,164.895004)..(289.718994,164.895004)
		..controls (196.440994,164.895004) and (77.312500,260.425995)..(73.546898,457.292999)
		..controls (72.656303,503.894989) and (144.983994,533.296997)..(150.628998,575.375000)
		..controls (156.875000,621.965027) and (107.987999,639.585999)..(102.460999,639.585999);
	endgroup;
);
picture SBE_pendant;
SBE_pendant := SBE_pendant_raw shifted (-(llcorner SBE_pendant_raw)-(urcorner SBE_pendant_raw - llcorner SBE_pendant_raw)/2) scaled (u / max((xpart urcorner SBE_pendant_raw) - (xpart llcorner SBE_pendant_raw), (ypart urcorner SBE_pendant_raw) - (ypart llcorner SBE_pendant_raw)));

def p_pendant_SBE(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_pendant;
enddef;


def p_pillarwithcurtains_UIS(expr pos, theta, sc, al) =
	U:=(.15u,.5u);
	T:=identity rotated theta aligned al scaled sc shifted pos;
	pickup PenC;
	thdraw (0,.25u)--(0,.1u){dir 180}..(-.1u,0)..{dir 0}(0,-.1u)--(0,-.25u);
	thdraw (-.15u,-.5u)--(0,-.25u)--(.15u,-.5u);
	thdraw (-.15u,.5u)--(0,.25u)--(.15u,.5u);
enddef;


def p_pillarswithcurtains_UIS(expr pos, theta, sc, al) =
	U:=(.3u,u);
	pickup PenC;
	pickup PenC;
	for i=-1 upto 1:
		T:=identity rotated theta aligned al scaled (sc * 0.7) shifted (pos + (i * .3u, abs(i) * .15u));
		thdraw (0,.25u)--(0,.1u){dir 180}..(-.1u,0)..{dir 0}(0,-.1u)--(0,-.25u);
		thdraw (-.15u,-.5u)--(0,-.25u)--(.15u,-.5u);
		thdraw (-.15u,.5u)--(0,.25u)--(.15u,.5u);
	endfor;
enddef;


picture SBE_photo_raw;
SBE_photo_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (196.261993,477.843994)..controls (190.938004,477.574005) and (190.414001,477.390991)..(188.386993,475.125000)
	    --(186.214996,472.695007)--(186.214996,445.718994)--(111.441002,445.718994)
	    --(108.515999,443.550995)..controls (103.637001,439.941010) and (75.367203,410.992004)..(74.511703,408.730011)
	    ..controls (73.980499,407.316010) and (73.714798,390.519989)..(73.714798,357.843994)
	    ..controls (73.714798,314.980011) and (73.859398,308.765991)..(74.898399,306.578003)
	    ..controls (75.546898,305.207001) and (76.644501,303.804993)..(77.335899,303.464996)
	    ..controls (78.023399,303.121002) and (102.719002,302.671997)..(132.214996,302.468994)
	    --(185.839996,302.093994)--(186.214996,296.355011)..controls (186.604996,290.378998) and (187.781006,288.039001)..(191.089996,286.671997)
	    --(191.089996,286.667999)..controls (191.914001,286.328003) and (222.401993,286.117004)..(258.835999,286.195007)
	    --(325.089996,286.343994)--(327.273010,288.792999)..controls (329.281006,291.035004) and (329.464996,291.710999)..(329.464996,296.855011)
	    --(329.464996,302.468994)--(436.828003,302.468994)--(439.023010,304.660004)
	    --(441.214996,306.851990)--(441.214996,409.234009)--(424.527008,426.148010)
	    ..controls (415.347992,435.453003) and (406.996002,443.660004)..(405.964996,444.386993)
	    ..controls (404.234009,445.605011) and (401.195007,445.707001)..(366.777008,445.710999)
	    --(329.464996,445.718994)--(329.464996,472.695007)--(327.292999,475.125000)
	    ..controls (325.265991,477.390991) and (324.738007,477.574005)..(319.417999,477.843994)
	    --cycle withcolor black;
	pickup pencircle scaled 14.173200bp;
	interim linecap := rounded;
	interim linejoin := beveled;
	draw (193.949005,500.894989)--(108.339996,620.598022) withcolor black;
	draw (320.472992,500.894989)--(406.082001,620.598022) withcolor black;
	endgroup;
);
picture SBE_photo;
SBE_photo := SBE_photo_raw shifted (-(llcorner SBE_photo_raw)-(urcorner SBE_photo_raw - llcorner SBE_photo_raw)/2) scaled (u / max((xpart urcorner SBE_photo_raw) - (xpart llcorner SBE_photo_raw), (ypart urcorner SBE_photo_raw) - (ypart llcorner SBE_photo_raw)));

def p_photo_SBE(expr pos, theta, sc, al) = 
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_photo;
enddef;


picture UIS_plus_raw;
UIS_plus_raw := image(
	fill (394.687988,411.277008)--(394.687988,403.121002)--(402.843994,403.121002)
		--(402.843994,400.621002)--(394.687988,400.621002)--(394.687988,392.464996)
		--(392.234009,392.464996)--(392.234009,400.621002)--(384.062988,400.621002)
		--(384.062988,403.121002)--(392.234009,403.121002)--(392.234009,411.277008)
		--cycle withcolor black;
	pickup pencircle scaled 0.750000bp;
	draw (394.687988,411.277008)--(394.687988,403.121002)--(402.843994,403.121002)
		--(402.843994,400.621002)--(394.687988,400.621002)--(394.687988,392.464996)
		--(392.234009,392.464996)--(392.234009,400.621002)--(384.062988,400.621002)
		--(384.062988,403.121002)--(392.234009,403.121002)--(392.234009,411.277008)
		--cycle withcolor black;
	pickup pencircle scaled 1.333610bp;
	draw (407.531006,401.867004)..controls (407.531006,394.097992) and (401.230011,387.796997)..(393.460999,387.796997)
		..controls (385.687988,387.796997) and (379.390991,394.097992)..(379.390991,401.867004)
		..controls (379.390991,409.636993) and (385.687988,415.937988)..(393.460999,415.937988)
		..controls (401.230011,415.937988) and (407.531006,409.636993)..(407.531006,401.867004)
		--cycle withcolor black;
);
picture UIS_plus;
UIS_plus := UIS_plus_raw shifted (-(llcorner UIS_plus_raw)-(urcorner UIS_plus_raw - llcorner UIS_plus_raw)/2) scaled (u / max((xpart urcorner UIS_plus_raw) - (xpart llcorner UIS_plus_raw), (ypart urcorner UIS_plus_raw) - (ypart llcorner UIS_plus_raw)));

def p_plus_UIS(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw UIS_plus;
enddef;


picture UIS_plus_minus_raw;
UIS_plus_minus_raw := image(
	fill (482.812988,451.992004)--(482.812988,443.835999)--(490.968994,443.835999)
		--(490.968994,441.335999)--(482.812988,441.335999)--(482.812988,433.179993)
		--(480.359009,433.179993)--(480.359009,441.335999)--(472.187988,441.335999)
		--(472.187988,443.835999)--(480.359009,443.835999)--(480.359009,451.992004)
		--cycle withcolor black;
	pickup pencircle scaled 0.750000bp;
	draw (482.812988,451.992004)--(482.812988,443.835999)--(490.968994,443.835999)
		--(490.968994,441.335999)--(482.812988,441.335999)--(482.812988,433.179993)
		--(480.359009,433.179993)--(480.359009,441.335999)--(472.187988,441.335999)
		--(472.187988,443.835999)--(480.359009,443.835999)--(480.359009,451.992004)
		--cycle withcolor black;
	pickup pencircle scaled 0.000000bp;
	fill (475.089996,425.414001)--(488.074005,425.414001)--(488.074005,421.460999)
		--(475.089996,421.460999)--cycle withcolor black;
	pickup pencircle scaled 1.332280bp;
	draw (481.585999,412.011993)..controls (473.812988,412.011993) and (467.515991,418.312988)..(467.515991,426.082001)
		--(467.515991,442.582001)..controls (467.515991,450.351990) and (473.812988,456.652008)..(481.585999,456.652008)
		..controls (489.355011,456.652008) and (495.656006,450.351990)..(495.656006,442.582001)
		--(495.656006,426.082001)..controls (495.656006,418.312988) and (489.355011,412.011993)..(481.585999,412.011993)
		--cycle withcolor black;
);
picture UIS_plus_minus;
UIS_plus_minus := UIS_plus_minus_raw shifted (-(llcorner UIS_plus_minus_raw)-(urcorner UIS_plus_minus_raw - llcorner UIS_plus_minus_raw)/2) scaled (u / ((xpart urcorner UIS_plus_minus_raw) - (xpart llcorner UIS_plus_minus_raw)));

def p_plusminus_UIS(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw UIS_plus_minus;
enddef;


picture SBE_seed_germination_raw;
SBE_seed_germination_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (307.054993,644.315979)..controls (300.976990,644.448975) and (294.867004,639.804993)..(294.738007,630.190979)
		--(294.335999,599.620972)..controls (284.722992,608.976990) and (274.328003,617.726990)..(262.230011,623.710999)
		..controls (233.117004,639.257996) and (197.210999,637.718994)..(168.223007,622.632996)
		..controls (168.651993,602.362976) and (174.352005,582.090027)..(185.996002,565.354980)
		..controls (208.809006,530.617004) and (253.582001,512.960999)..(294.003998,522.429993)
		--(294.472992,470.847992)--(266.761993,439.105011)--(306.425995,346.949005)
		--(346.359009,438.929993)--(319.855011,469.644989)--(319.179993,502.457001)
		..controls (319.179993,502.457001) and (334.320007,493.828003)..(350.433990,495.203003)
		..controls (400.753998,499.503998) and (444.437988,544.109009)..(444.414001,594.953003)
		..controls (435.062988,605.223022) and (415.863007,608.715027)..(401.238007,609.000000)
		..controls (370.246002,611.851990) and (339.906006,597.054993)..(319.179993,574.590027)
		--(319.179993,629.757996)..controls (319.179993,639.273010) and (313.132996,644.184021)..(307.054993,644.315979)
		--cycle;
	fill (378.417999,450.589996)--(317.867004,297.796997)..controls (313.351990,286.406006) and (305.851990,284.824005)..(299.785004,298.843994)
		--(234.445007,449.886993)..controls (205.804993,427.437988) and (190.828003,389.375000)..(194.429993,353.316010)
		..controls (197.300995,318.671997) and (218.117004,287.230011)..(247.104996,268.855011)
		--(375.769989,275.878998)..controls (390.304993,287.136993) and (402.128998,301.910004)..(409.453003,319.289001)
		..controls (429.414001,363.664001) and (416.367004,419.933990)..(378.417999,450.589996)
		--cycle;
	pickup pencircle scaled 17.511600bp;
	draw (101.250000,276.343994)..controls (177.468994,243.992004) and (396.476990,296.585999)..(502.667999,269.492004);
	endgroup;
);
picture SBE_seed_germination;
SBE_seed_germination := SBE_seed_germination_raw shifted (-(llcorner SBE_seed_germination_raw)-(urcorner SBE_seed_germination_raw - llcorner SBE_seed_germination_raw)/2) scaled (u / max((xpart urcorner SBE_seed_germination_raw) - (xpart llcorner SBE_seed_germination_raw), (ypart urcorner SBE_seed_germination_raw) - (ypart llcorner SBE_seed_germination_raw)));

def p_seedgermination_SBE(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_seed_germination;
enddef;


def p_stalactitestalagmite_UIS(expr pos,theta,sc,al) =
	U:=(.15u, .4u);
	T:=identity rotated theta aligned al scaled sc shifted pos;
	pickup PenC;
	thdraw (-.15u,.4u) -- (0,.2u) -- (.15u,.4u);
	thdraw (.15u,-.4u) -- (0,-.2u) -- (-.15u,-.4u);
	thdraw (0,-.2u) -- (0,-.05u);
	thdraw (0,.2u) -- (0,.05u);
enddef;


def p_stalactitesstalagmites_UIS(expr pos,theta,sc,al) =
	U:=(.3u, .8u);
	pickup PenC;
	for i=-1 upto 1:
		T:=identity rotated theta aligned al scaled (sc * 0.7) shifted (pos + (i * .3u, abs(i) * .15u));
		thdraw (-.15u,.4u) -- (0,.2u) -- (.15u,.4u);
		thdraw (.15u,-.4u) -- (0,-.2u) -- (-.15u,-.4u);
		thdraw (0,-.2u) -- (0,-.05u);
		thdraw (0,.2u) -- (0,.05u);
	endfor;
enddef;


picture SBE_tree_trunk_raw;
SBE_tree_trunk_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (223.335999,206.113007)..controls (172.589996,211.957001) and (132.348007,248.246002)..(122.214996,297.292999)
		..controls (114.875000,332.824005) and (124.875000,370.128998)..(148.983994,397.140991)
		--(382.371002,582.289001)..controls (387.332001,585.539001) and (393.710999,578.987976)..(390.109009,574.343994)
		--(155.617004,387.167999)..controls (145.348007,374.753998) and (138.593994,361.570007)..(134.348007,345.644989)
		..controls (132.147995,337.398010) and (132.042999,336.343994)..(131.996002,321.644989)
		..controls (131.940994,305.058990) and (132.651993,299.835999)..(136.546997,288.128998)
		..controls (144.358994,264.656006) and (161.320007,243.809006)..(183.016006,231.003998)
		..controls (192.949005,225.141006) and (207.378998,220.074005)..(219.695007,218.121002)
		..controls (227.492004,216.886993) and (244.050995,217.082001)..(252.320007,218.507996)
		..controls (273.097992,222.085999) and (293.187988,232.574005)..(308.238007,247.699005)
		--(489.460999,476.093994)..controls (493.562988,478.398010) and (498.925995,473.335999)..(496.957001,469.019989)
		--(326.375000,250.934006)..controls (316.753998,239.003998) and (309.160004,231.891006)..(298.398010,224.738007)
		..controls (286.390991,216.753998) and (268.554993,209.664001)..(254.598007,207.324005)
		..controls (245.925995,205.871002) and (230.582001,205.276993)..(223.335999,206.113007)
		--cycle;
	fill (219.261993,237.651993)..controls (192.348007,242.207001) and (168.414001,260.894989)..(157.723007,285.691010)
		..controls (153.089996,296.433990) and (151.742004,302.605011)..(151.285004,315.136993)
		..controls (150.828003,327.800995) and (151.636993,334.390991)..(154.863007,344.269989)
		..controls (159.000000,356.929993) and (165.309006,367.261993)..(174.695007,376.738007)
		..controls (212.621002,415.019989) and (276.734009,406.746002)..(303.976990,360.054993)
		..controls (315.027008,341.113007) and (317.964996,317.359009)..(311.859009,296.296997)
		..controls (303.140991,266.207001) and (278.640991,243.820007)..(248.007996,237.960999)
		..controls (242.093994,236.828003) and (225.199005,236.647995)..(219.261993,237.651993)
		--cycle;
	fill (371.914001,396.238007)--(437.273010,472.726990)..controls (441.355011,474.332001) and (446.117004,469.890991)..(444.609009,465.882996)
		--(377.296997,394.843994)..controls (374.605011,394.125000) and (373.824005,394.324005)..(371.914001,396.238007)
		--cycle;
	fill (281.343994,435.730011)..controls (279.148010,437.007996) and (278.429993,439.871002)..(279.621002,442.621002)
		--(349.585999,511.394989)..controls (352.613007,511.394989) and (354.949005,509.390991)..(355.550995,506.261993)
		--(284.937988,434.917999)..controls (283.703003,434.929993) and (282.085999,435.296997)..(281.343994,435.730011)
		--cycle;
	fill (386.296997,487.449005)..controls (384.804993,488.625000) and (384.320007,489.675995)..(384.320007,491.753998)
		--(454.964996,562.395020)..controls (457.039001,562.395020) and (458.093994,561.914001)..(459.265991,560.418030)
		--cycle;
	endgroup;
);
picture SBE_tree_trunk;
SBE_tree_trunk := SBE_tree_trunk_raw shifted (-(llcorner SBE_tree_trunk_raw)-(urcorner SBE_tree_trunk_raw - llcorner SBE_tree_trunk_raw)/2) scaled (u / max((xpart urcorner SBE_tree_trunk_raw) - (xpart llcorner SBE_tree_trunk_raw), (ypart urcorner SBE_tree_trunk_raw) - (ypart llcorner SBE_tree_trunk_raw)));

def p_treetrunk_SBE(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_tree_trunk;
enddef;


picture SBE_volcano_raw;
SBE_volcano_raw := image(
	pickup pencircle scaled 21.160601bp;
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	draw (36.671902,222.897995)--(169.684006,567.409973)..controls (177.815994,510.679993) and (212.089996,485.109009)..(265.714996,485.109009)
		..controls (319.335999,485.109009) and (353.609009,510.679993)..(361.742004,567.409973)
		--(494.753998,222.897995);
	endgroup;
);
picture SBE_volcano;
SBE_volcano := SBE_volcano_raw shifted (-(llcorner SBE_volcano_raw)-(urcorner SBE_volcano_raw - llcorner SBE_volcano_raw)/2) scaled (u / max((xpart urcorner SBE_volcano_raw) - (xpart llcorner SBE_volcano_raw), (ypart urcorner SBE_volcano_raw) - (ypart llcorner SBE_volcano_raw)));

def p_volcano_SBE(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_volcano;
enddef;


picture SBE_walkway_raw;
SBE_walkway_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (100.746002,152.141006)..controls (107.078003,151.656006) and (112.616997,156.395004)..(113.121002,162.688004)
		..controls (113.625000,169.022995) and (108.905998,174.559006)..(102.573997,175.063004)
		..controls (96.242203,175.546997) and (90.683601,170.848007)..(90.199203,164.516006)
		..controls (89.714798,158.184006) and (94.433601,152.625000)..(100.746002,152.141006)
		--cycle;
	fill (91.015602,147.438004)..controls (93.269501,148.992004) and (96.066399,150.059006)..(99.078102,149.824005)
		..controls (103.019997,149.477005) and (106.285004,147.046997)..(108.226997,144.054993)
		--(119.844002,121.078003)--(135.656006,110.180000)..controls (137.016006,109.148003) and (137.832001,107.441002)..(137.695007,105.633003)
		..controls (137.479996,102.777000) and (134.977005,100.660004)..(132.102005,100.894997)
		..controls (131.246002,100.914001) and (130.565994,101.262001)..(129.729996,101.652000)
		--(112.460999,113.519997)..controls (111.938004,113.964996) and (111.527000,114.473000)..(111.180000,115.073997)
		--(106.808998,123.680000)--(101.640999,100.737999)--(122.039001,76.632797)
		..controls (122.484001,75.875000) and (122.796997,75.039101)..(122.987999,74.183601)
		--(128.488007,45.066399)..controls (128.468994,44.425800) and (128.565994,44.015598)..(128.526993,43.472698)
		..controls (128.175995,39.199200) and (124.426003,36.011700)..(120.133003,36.343800)
		..controls (116.598000,36.613300) and (113.957001,39.277302)..(113.121002,42.500000)
		--(107.914001,69.773399)--(91.382797,87.917999)--(87.500000,70.222702)
		..controls (87.382797,69.386703) and (86.296898,67.656303)..(85.945297,66.917999)
		--(70.074203,40.152302)..controls (68.460899,37.683601) and (65.820297,36.128899)..(62.769501,36.343800)
		..controls (58.476601,36.691399) and (55.273399,40.441399)..(55.621101,44.734402)
		..controls (55.718800,45.937500) and (56.226601,47.203098)..(56.652302,48.074200)
		--(71.453102,72.882797)--(83.750000,127.527000)--(75.707001,121.000000)
		--(71.453102,101.535004)..controls (70.890602,99.066399) and (68.617203,97.007797)..(65.917999,97.242203)
		..controls (63.082001,97.476601) and (60.925800,99.941399)..(61.140598,102.816002)
		--(61.253899,103.496002)--(66.324203,126.183998)..controls (66.597702,127.214996) and (67.160202,128.050995)..(67.937500,128.690994)
		--cycle;
	fill (18.835899,65.625000)--(30.664101,65.625000)--(30.664101,56.226601)
		--(18.835899,56.226601)--cycle;
	fill (42.203098,65.625000)--(54.035198,65.625000)--(54.035198,56.226601)
		--(42.203098,56.226601)--cycle;
	fill (89.250000,65.625000)--(101.082001,65.625000)--(101.082001,56.226601)
		--(89.250000,56.226601)--cycle;
	fill (136.608994,65.625000)--(148.440994,65.625000)--(148.440994,56.226601)
		--(136.608994,56.226601)--cycle;
	fill (159.979996,65.625000)--(171.809006,65.625000)--(171.809006,56.226601)
		--(159.979996,56.226601)--cycle;
	fill (18.835899,31.339800)--(30.664101,31.339800)--(30.664101,21.937500)
		--(18.835899,21.937500)--cycle;
	fill (42.203098,31.339800)--(54.035198,31.339800)--(54.035198,21.937500)
		--(42.203098,21.937500)--cycle;
	fill (65.882797,31.339800)--(77.714798,31.339800)--(77.714798,21.937500)
		--(65.882797,21.937500)--cycle;
	fill (113.241997,31.339800)--(125.070000,31.339800)--(125.070000,21.937500)
		--(113.241997,21.937500)--cycle;
	fill (89.250000,31.339800)--(101.082001,31.339800)--(101.082001,21.937500)
		--(89.250000,21.937500)--cycle;
	fill (136.608994,31.339800)--(148.440994,31.339800)--(148.440994,21.937500)
		--(136.608994,21.937500)--cycle;
	fill (159.979996,31.339800)--(171.809006,31.339800)--(171.809006,21.937500)
		--(159.979996,21.937500)--cycle;
	endgroup;
);
picture SBE_walkway;
SBE_walkway := SBE_walkway_raw shifted (-(llcorner SBE_walkway_raw)-(urcorner SBE_walkway_raw - llcorner SBE_walkway_raw)/2) scaled (u / max((xpart urcorner SBE_walkway_raw) - (xpart llcorner SBE_walkway_raw), (ypart urcorner SBE_walkway_raw) - (ypart llcorner SBE_walkway_raw)));

def p_walkway_SBE(expr pos, theta, sc, al) = 
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_walkway;
enddef;


picture SBE_water_drip_raw;
SBE_water_drip_raw := image(
	% Central
	fill (255.750000,242.003998)..controls (228.809006,245.266006) and (206.550995,257.015991)..(187.141006,278.226990)
		..controls (173.824005,292.773010) and (168.175995,305.441010)..(166.531006,324.441010)
		..controls (165.164001,340.191010) and (168.300995,359.296997)..(174.742004,374.488007)
		..controls (178.977005,384.472992) and (192.636993,410.996002)..(217.358994,457.234009)
		..controls (245.479996,509.828003) and (251.796997,523.370972)..(259.386993,547.301025)
		..controls (264.132996,562.265991) and (269.250000,585.815979)..(269.250000,592.684021)
		..controls (269.250000,594.085999) and (269.585999,595.234009)..(270.000000,595.234009)
		..controls (270.414001,595.234009) and (270.750000,594.085999)..(270.750000,592.684021)
		..controls (270.750000,585.815979) and (275.867004,562.265991)..(280.613007,547.301025)
		..controls (288.191010,523.398010) and (294.562988,509.746002)..(322.355011,457.808990)
		..controls (347.242004,411.304993) and (362.722992,381.085999)..(366.281006,372.070007)
		..controls (375.703003,348.175995) and (376.230011,320.277008)..(367.648010,299.847992)
		..controls (364.171997,291.574005) and (358.781006,284.265991)..(348.750000,274.230011)
		..controls (331.449005,256.917999) and (313.117004,246.996002)..(291.195007,243.070007)
		..controls (283.292999,241.651993) and (263.550995,241.063004)..(255.750000,242.003998)
		--cycle withcolor black;

	% Reflex in central
	fill (292.500000,261.429993)..controls (302.691010,263.691010) and (312.746002,267.984009)..(321.800995,273.949005)
		..controls (338.093994,284.675995) and (349.867004,299.484009)..(354.851990,315.515991)
		..controls (357.503998,324.054993) and (357.359009,336.605011)..(354.503998,346.191010)
		..controls (348.816010,365.265991) and (336.257996,391.210999)..(320.726990,415.984009)
		..controls (314.636993,425.699005) and (306.046997,438.484009)..(305.609009,438.484009)
		..controls (305.414001,438.484009) and (307.566010,433.843994)..(310.398010,428.171997)
		..controls (323.867004,401.175995) and (332.582001,375.285004)..(335.703003,353.011993)
		..controls (337.457001,340.515991) and (336.816010,322.972992)..(334.246002,313.082001)
		..controls (331.996002,304.433990) and (327.542999,294.605011)..(322.976990,288.226990)
		..controls (314.687988,276.648010) and (301.199005,266.945007)..(286.355011,261.890991)
		..controls (281.683990,260.296997) and (281.261993,260.023010)..(283.500000,260.019989)
		..controls (284.945007,260.019989) and (288.992004,260.652008)..(292.500000,261.429993)
		--cycle withcolor white;

	% Lower left
	fill (129.750000,262.656006)..controls (119.155998,264.937988) and (112.589996,270.269989)..(111.930000,277.117004)
		..controls (111.448997,282.128998) and (112.839996,285.863007)..(116.578003,289.601990)
		..controls (124.438004,297.460999) and (136.324005,294.691010)..(146.901993,282.527008)
		..controls (152.003998,276.664001) and (158.457001,271.738007)..(165.050995,268.687988)
		--(170.332001,266.242004)--(162.977005,264.605011)..controls (148.156006,261.308990) and (138.598007,260.750000)..(129.750000,262.656006)
		--cycle withcolor black;

	% Lower right
	fill (388.875000,262.257996)..controls (386.398010,262.632996) and (381.066010,263.683990)..(377.023010,264.593994)
		--(369.667999,266.242004)--(374.949005,268.687988)..controls (381.593994,271.765991) and (388.027008,276.687988)..(393.148010,282.617004)
		..controls (400.546997,291.179993) and (409.562988,295.433990)..(416.574005,293.667999)
		..controls (425.679993,291.375000) and (430.832001,280.738007)..(426.730011,272.695007)
		..controls (423.394989,266.156006) and (414.281006,262.273010)..(401.250000,261.835999)
		..controls (396.917999,261.687988) and (391.351990,261.878998)..(388.875000,262.257996)
		--cycle withcolor black;

	% Upper left
	fill (155.035004,298.992004)..controls (148.757996,303.933990) and (134.410004,308.132996)..(113.109001,311.269989)
		..controls (96.882797,313.656006) and (92.835899,315.078003)..(87.085899,320.398010)
		..controls (80.851601,326.171997) and (78.421898,331.726990)..(78.414101,340.234009)
		..controls (78.406303,347.992004) and (80.539101,353.312988)..(85.742203,358.519989)
		..controls (89.855499,362.628998) and (93.648399,364.628998)..(100.296997,366.191010)
		..controls (121.258003,371.121002) and (140.535004,353.421997)..(151.897995,318.828003)
		..controls (154.488007,310.941010) and (157.718994,298.207001)..(157.253998,297.738007)
		..controls (157.102005,297.585999) and (156.104996,298.152008)..(155.035004,298.992004)
		--cycle withcolor black;

	% Upper right
	fill (382.500000,297.710999)..controls (382.500000,299.195007) and (386.046997,312.566010)..(388.101990,318.828003)
		..controls (399.464996,353.421997) and (418.742004,371.121002)..(439.703003,366.191010)
		..controls (446.351990,364.628998) and (450.144989,362.628998)..(454.257996,358.519989)
		..controls (459.460999,353.312988) and (461.593994,347.992004)..(461.585999,340.234009)
		..controls (461.578003,331.726990) and (459.148010,326.171997)..(452.914001,320.398010)
		..controls (447.164001,315.078003) and (443.117004,313.656006)..(426.890991,311.269989)
		..controls (405.535004,308.125000) and (391.554993,303.957001)..(384.187988,298.542999)
		..controls (383.257996,297.859009) and (382.500000,297.484009)..(382.500000,297.710999)
		--cycle withcolor black;
);
picture SBE_water_drip;
SBE_water_drip := SBE_water_drip_raw shifted (-(llcorner SBE_water_drip_raw)-(urcorner SBE_water_drip_raw - llcorner SBE_water_drip_raw)/2) scaled (u / max((xpart urcorner SBE_water_drip_raw) - (xpart llcorner SBE_water_drip_raw), (ypart urcorner SBE_water_drip_raw) - (ypart llcorner SBE_water_drip_raw)));

def p_waterdrip_SBE(expr pos, theta, sc, al) =
	T:=identity aligned al rotated theta scaled sc shifted pos;
	thdraw SBE_water_drip;
enddef;


picture SBE_wheelchair_raw;
SBE_wheelchair_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (260.250000,282.113007)..controls (248.195007,283.492004) and (243.151993,284.300995)..(236.863007,285.871002)
		..controls (185.647995,298.648010) and (145.852005,340.722992)..(137.608994,390.808990)
		..controls (128.695007,444.988007) and (155.022995,497.621002)..(204.750000,525.034973)
		..controls (218.397995,532.554993) and (225.742004,534.898010)..(231.688004,533.625000)
		..controls (240.770004,531.676025) and (246.276993,523.551025)..(244.414001,514.851990)
		..controls (243.272995,509.546997) and (239.714996,505.906006)..(232.733994,502.910004)
		..controls (203.906006,490.539001) and (183.820007,469.671997)..(174.104996,442.011993)
		..controls (170.434006,431.566010) and (169.188004,423.738007)..(169.223007,411.433990)
		..controls (169.276993,394.613007) and (172.070007,382.855011)..(179.453003,368.398010)
		..controls (193.070007,341.734009) and (218.542999,322.542999)..(249.375000,315.722992)
		..controls (257.585999,313.906006) and (277.851990,313.507996)..(286.500000,314.992004)
		..controls (326.503998,321.859009) and (357.753998,349.492004)..(368.660004,387.632996)
		..controls (370.539001,394.207001) and (372.628998,397.679993)..(376.156006,400.097992)
		..controls (381.507996,403.765991) and (387.429993,403.855011)..(393.906006,400.375000)
		..controls (402.406006,395.808990) and (403.757996,388.308990)..(398.738007,373.558990)
		..controls (395.742004,364.742004) and (389.175995,350.457001)..(385.296997,344.308990)
		..controls (365.308990,312.625000) and (332.300995,290.769989)..(294.000000,283.851990)
		..controls (286.496002,282.496002) and (266.089996,281.445007)..(260.250000,282.113007)
		--cycle;
	fill (467.046997,312.886993)..controls (460.328003,314.953003) and (459.062988,316.832001)..(429.351990,368.847992)
		--(401.625000,417.390991)--(350.574005,417.410004)..controls (294.496002,417.433990) and (295.515991,417.359009)..(288.488007,422.011993)
		..controls (283.910004,425.042999) and (280.011993,430.167999)..(278.609009,435.000000)
		..controls (276.437988,442.480011) and (255.000000,547.281006)..(255.000000,550.418030)
		..controls (255.000000,561.828003) and (262.167999,572.448975)..(273.425995,577.723022)
		..controls (277.445007,579.604980) and (278.671997,579.804993)..(286.125000,579.804993)
		..controls (293.894989,579.804993) and (294.664001,579.664001)..(299.304993,577.395020)
		..controls (305.285004,574.468994) and (311.304993,568.382996)..(313.964996,562.573975)
		..controls (314.996002,560.312988) and (316.761993,553.648010)..(317.878998,547.762024)
		--(319.914001,537.054993)--(388.125000,536.304993)--(391.703003,534.203003)
		..controls (396.632996,531.304993) and (399.203003,527.210999)..(399.609009,521.612976)
		..controls (400.128998,514.421997) and (397.351990,509.363007)..(391.207001,506.300995)
		--(387.457001,504.429993)--(326.972992,504.429993)--(327.367004,502.742004)
		..controls (327.582001,501.816010) and (329.292999,493.460999)..(331.167999,484.179993)
		..controls (333.039001,474.898010) and (334.757996,466.558990)..(334.984009,465.644989)
		--(335.398010,463.984009)--(373.781006,463.769989)--(412.160004,463.554993)
		--(417.015991,461.164001)..controls (419.687988,459.847992) and (422.953003,457.652008)..(424.273010,456.289001)
		..controls (427.167999,453.292999) and (490.937988,342.886993)..(492.855011,337.554993)
		..controls (495.515991,330.144989) and (493.949005,323.308990)..(488.285004,317.644989)
		..controls (482.695007,312.054993) and (475.207001,310.378998)..(467.046997,312.886993)
		--cycle;
	fill (265.523010,590.382996)..controls (252.574005,593.862976) and (241.207001,605.551025)..(238.147995,618.534973)
		..controls (235.007996,631.851990) and (238.602005,644.034973)..(248.625000,654.059021)
		..controls (257.054993,662.492004) and (267.546997,666.476990)..(278.589996,665.445007)
		..controls (291.546997,664.237976) and (304.082001,655.718994)..(309.386993,644.520020)
		..controls (316.609009,629.265991) and (313.773010,612.031006)..(302.121002,600.395020)
		..controls (299.457001,597.734009) and (295.539001,594.612976)..(293.414001,593.460999)
		..controls (286.046997,589.465027) and (273.917999,588.125000)..(265.523010,590.382996)
		--cycle;
	endgroup;
);
picture SBE_wheelchair;
SBE_wheelchair := SBE_wheelchair_raw shifted (-(llcorner SBE_wheelchair_raw)-(urcorner SBE_wheelchair_raw - llcorner SBE_wheelchair_raw)/2) scaled (u / max((xpart urcorner SBE_wheelchair_raw) - (xpart llcorner SBE_wheelchair_raw), (ypart urcorner SBE_wheelchair_raw) - (ypart llcorner SBE_wheelchair_raw)));

def p_wheelchair_SBE(expr pos, theta, sc, al) =
	T:=identity aligned al rotated theta scaled sc shifted pos;
	thdraw SBE_wheelchair;
enddef;
