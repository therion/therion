% Symbol set of Brazilian Speleological Society (SBE - Sociedade Brasileira de
% Espeleologia), author Rodrigo Severo.


%%%%%%%%%%%%%%%%%%%%%%%%% Points %%%%%%%%%%%%%%%%%%%%%%%%%


picture SBE_altar_raw;
SBE_altar_raw := image(
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
);

def p_altar_SBE(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_altar_raw shifted (-(llcorner SBE_altar_raw)-(urcorner SBE_altar_raw - llcorner SBE_altar_raw)/2) scaled (u / max((xpart urcorner SBE_altar_raw) - (xpart llcorner SBE_altar_raw), (ypart urcorner SBE_altar_raw) - (ypart llcorner SBE_altar_raw)));
enddef;


picture SBE_archeo_excavation_raw;
SBE_archeo_excavation_raw := image(
	begingroup;
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
);

def p_audio_SBE(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_audio_raw shifted (-(llcorner SBE_audio_raw)-(urcorner SBE_audio_raw - llcorner SBE_audio_raw)/2) scaled (u / max((xpart urcorner SBE_audio_raw) - (xpart llcorner SBE_audio_raw), (ypart urcorner SBE_audio_raw) - (ypart llcorner SBE_audio_raw)));
enddef;


picture SBE_bat_raw;
SBE_bat_raw := image(
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
);

def p_bat_SBE(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_bat_raw shifted (-(llcorner SBE_bat_raw)-(urcorner SBE_bat_raw - llcorner SBE_bat_raw)/2) scaled (u / max((xpart urcorner SBE_bat_raw) - (xpart llcorner SBE_bat_raw), (ypart urcorner SBE_bat_raw) - (ypart llcorner SBE_bat_raw)));
enddef;


picture SBE_bones_raw;
SBE_bones_raw := image(
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
);

def p_mud_SBE(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_mud_raw shifted (-(llcorner SBE_mud_raw)-(urcorner SBE_mud_raw - llcorner SBE_mud_raw)/2) scaled (u / max((xpart urcorner SBE_mud_raw) - (xpart llcorner SBE_mud_raw), (ypart urcorner SBE_mud_raw) - (ypart llcorner SBE_mud_raw)));
enddef;


picture SBE_mud_crack_raw;
SBE_mud_crack_raw := image(
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


picture SBE_wheelchair_raw;
SBE_wheelchair_raw := image(
	begingroup;
		fill (363,192)..controls (363,97) and (286,20)..(192,20)
			..controls (97,20) and (20,97)..(20,192)
			..controls (20,287) and (97,364)..(192,364)
			..controls (286,364) and (363,287)..(363,192)
			--cycle withcolor white;
		pickup pencircle scaled 39bp;
		interim linecap := rounded;
		interim linejoin := rounded;
		draw (363,192)..controls (363,97) and (286,20)..(192,20)
			..controls (97,20) and (20,97)..(20,192)
			..controls (20,287) and (97,364)..(192,364)
			..controls (286,364) and (363,287)..(363,192)
			--cycle withcolor red;
		fill (202,301)..controls (202,286) and (190,274)..(175,274)
			..controls (160,274) and (148,286)..(148,301)
			..controls (148,316) and (160,328)..(175,328)
			..controls (190,328) and (202,316)..(202,301)
			--cycle;
		pickup pencircle scaled 21bp;
		draw (139,221)..controls (109,212) and (88,184)..(88,152)
			..controls (88,113) and (120,81)..(160,81)
			..controls (192,81) and (221,102)..(229,133);
		draw (177,254)--(177,178)--(259,178)
			--(278,129)--(299,134);
		draw (177,233)--(239,233);
	endgroup;
);

picture SBE_nowheelchair_raw;
SBE_nowheelchair_raw := image(
	draw SBE_wheelchair_raw;
	begingroup;
		pickup pencircle scaled 30bp;
		interim linecap := butt;
		interim linejoin := mitered;
		draw (60,307)--(323,77) withcolor red;
	endgroup;
);

def p_nowheelchair_SBE(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_nowheelchair_raw shifted (-(llcorner SBE_nowheelchair_raw)-(urcorner SBE_nowheelchair_raw - llcorner SBE_nowheelchair_raw)/2) scaled (u / max((xpart urcorner SBE_nowheelchair_raw) - (xpart llcorner SBE_nowheelchair_raw), (ypart urcorner SBE_nowheelchair_raw) - (ypart llcorner SBE_nowheelchair_raw)));
enddef;


picture SBE_pendant_raw;
SBE_pendant_raw := image(
	pickup pencircle scaled 22bp;
	begingroup;
		interim linecap := butt;
		interim linejoin := mitered;
		draw (476,639)..controls (471,639) and (422,621)..(428,575)
			..controls (434,533) and (506,503)..(505,457)
			..controls (502,260) and (383,164)..(289,164)
			..controls (196,164) and (77,260)..(73,457)
			..controls (72,503) and (144,533)..(150,575)
			..controls (156,621) and (107,639)..(102,639);
	endgroup;
);

def p_pendant_SBE(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_pendant_raw shifted (-(llcorner SBE_pendant_raw)-(urcorner SBE_pendant_raw - llcorner SBE_pendant_raw)/2) scaled (u / max((xpart urcorner SBE_pendant_raw) - (xpart llcorner SBE_pendant_raw), (ypart urcorner SBE_pendant_raw) - (ypart llcorner SBE_pendant_raw)));
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
		fill (186,476)--(186,445)--(111,445)
			--(75,408)--(75,302)--(188,302)
			--(188,286)--(329,286)--(329,302)
			--(440,302)--(440,408)--(405,445)
			--(328,445)--(328,476)--cycle;
		pickup pencircle scaled 18bp;
		interim linecap := rounded;
		draw (194,499)--(107,621);
		draw (320,499)--(407,621);
	endgroup;
);

def p_photo_SBE(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_photo_raw shifted (-(llcorner SBE_photo_raw)-(urcorner SBE_photo_raw - llcorner SBE_photo_raw)/2) scaled (u / max((xpart urcorner SBE_photo_raw) - (xpart llcorner SBE_photo_raw), (ypart urcorner SBE_photo_raw) - (ypart llcorner SBE_photo_raw)));
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
		fill (443,464)..controls (441,492) and (411,512)..(389,501)
			..controls (406,574) and (355,622)..(378,509)
			..controls (350,513) and (331,473)..(333,452)
			..controls (350,440) and (374,461)..(381,474)
			..controls (382,466) and (417,441)..(443,464)
			--cycle;
		pickup pencircle scaled 25bp;
		draw (101,276)..controls (177,243) and (396,296)..(502,269);
		pickup pencircle scaled 19bp;
		interim linecap := rounded;
		draw (194,270)..controls (160,669) and (364,759)..(382,543);
	endgroup;
);

def p_seedgermination_SBE(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_seed_germination_raw shifted (-(llcorner SBE_seed_germination_raw)-(urcorner SBE_seed_germination_raw - llcorner SBE_seed_germination_raw)/2) scaled (u / max((xpart urcorner SBE_seed_germination_raw) - (xpart llcorner SBE_seed_germination_raw), (ypart urcorner SBE_seed_germination_raw) - (ypart llcorner SBE_seed_germination_raw)));
enddef;


def p_stalactitestalagmite_UIS(expr pos,theta,sc,al) =
	U:=(.15u, .4u);
	T:=identity rotated theta aligned al scaled sc shifted pos;
	pickup PenC;
	thdraw (-.15u,.4u) -- (0,.2u) -- (.15u,.4u);
	thdraw (.15u,-.4u) -- (0,-.2u) -- (-.15u,-.4u);
	thdraw (0,-.2u) -- (0,-.1u);
	thdraw (0,.2u) -- (0,.1u);
enddef;


def p_stalactitesstalagmites_UIS(expr pos,theta,sc,al) =
	U:=(.3u, .8u);
	pickup PenC;
	for i=-1 upto 1:
		T:=identity rotated theta aligned al scaled (sc * 0.7) shifted (pos + (i * .3u, abs(i) * .15u));
		thdraw (-.15u,.4u) -- (0,.2u) -- (.15u,.4u);
		thdraw (.15u,-.4u) -- (0,-.2u) -- (-.15u,-.4u);
		thdraw (0,-.2u) -- (0,-.1u);
		thdraw (0,.2u) -- (0,.1u);
	endfor;
enddef;


picture SBE_tree_trunk_raw;
SBE_tree_trunk_raw := image(
	begingroup;
		pickup pencircle scaled 11bp;
		interim linejoin := mitered;
		draw (338,554)--(153,393)..controls (64,292) and (204,143)..(312,243)
			--(504,488);
		pickup pencircle scaled 0.1bp;
		fill (313,318)..controls (313,274) and (277,238)..(232,238)
			..controls (188,238) and (152,274)..(152,318)
			..controls (152,363) and (188,398)..(232,398)
			..controls (277,398) and (313,363)..(313,318)
			--cycle;
		pickup pencircle scaled 5bp;
		draw (290,453)--(348,506);
		draw (375,406)--(445,481);
		draw (316,397)--(348,429);
	endgroup;
);

def p_treetrunk_SBE(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_tree_trunk_raw shifted (-(llcorner SBE_tree_trunk_raw)-(urcorner SBE_tree_trunk_raw - llcorner SBE_tree_trunk_raw)/2) scaled (u / max((xpart urcorner SBE_tree_trunk_raw) - (xpart llcorner SBE_tree_trunk_raw), (ypart urcorner SBE_tree_trunk_raw) - (ypart llcorner SBE_tree_trunk_raw)));
enddef;


picture SBE_volcano_raw;
SBE_volcano_raw := image(
	pickup pencircle scaled 21bp;
	begingroup;
		interim linecap := butt;
		interim linejoin := mitered;
		draw (36,222)--(169,567)..controls (177,510) and (212,485)..(265,485)
			..controls (319,485) and (353,510)..(361,567)
			--(494,222);
	endgroup;
);

def p_volcano_SBE(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_volcano_raw shifted (-(llcorner SBE_volcano_raw)-(urcorner SBE_volcano_raw - llcorner SBE_volcano_raw)/2) scaled (u / max((xpart urcorner SBE_volcano_raw) - (xpart llcorner SBE_volcano_raw), (ypart urcorner SBE_volcano_raw) - (ypart llcorner SBE_volcano_raw)));
enddef;


picture SBE_walkway_raw;
SBE_walkway_raw := image(
	begingroup;
		pickup pencircle scaled 9bp;
		interim linecap := butt;
		interim linejoin := mitered;
		draw (19,26)--(171,26);
		draw (135,61)--(171,61);
		draw (19,61)--(55,61);
		draw (89,61)--(104,61);
		pickup pencircle scaled 12bp;
		interim linecap := rounded;
		draw (63,43)--(79,72)--(93,135);
		draw (124,44)--(115,72)--(93,97)
			--(100,133);
		pickup pencircle scaled 9bp;
		interim linejoin := rounded;
		draw (133,109)--(115,117)--(99,144)
			--(72,128)--(62,107);
		pickup pencircle scaled 0.1bp;
		fill (113,162)..controls (113,157) and (108,152)..(102,152)
			..controls (96,152) and (91,157)..(91,162)
			..controls (91,168) and (96,173)..(102,173)
			..controls (108,173) and (113,168)..(113,162)
			--cycle;
	endgroup;
);

def p_walkway_SBE(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw SBE_walkway_raw shifted (-(llcorner SBE_walkway_raw)-(urcorner SBE_walkway_raw - llcorner SBE_walkway_raw)/2) scaled (u / max((xpart urcorner SBE_walkway_raw) - (xpart llcorner SBE_walkway_raw), (ypart urcorner SBE_walkway_raw) - (ypart llcorner SBE_walkway_raw)));
enddef;


picture SBE_water_drip_raw;
SBE_water_drip_raw := image(
	fill (493,1196)..controls (535,1234) and (583,1266)..(635,1290)
		..controls (595,1233) and (566,1160)..(588,1091)
		..controls (622,981) and (733,912)..(757,796)
		..controls (789,684) and (728,556)..(622,509)
		..controls (524,461) and (398,489)..(328,572)
		..controls (278,629) and (259,710)..(272,785)
		..controls (293,942) and (375,1090)..(493,1196)
		--cycle;
	fill (495,1159)..controls (403,1066) and (329,950)..(299,821)
		..controls (287,761) and (285,694)..(314,637)
		..controls (354,571) and (425,522)..(502,512)
		..controls (513,511) and (550,509)..(522,518)
		..controls (449,551) and (384,613)..(366,694)
		..controls (342,794) and (371,898)..(411,990)
		..controls (441,1059) and (481,1123)..(524,1185)
		..controls (514,1177) and (505,1168)..(495,1159)
		--cycle withcolor white;
	fill (353,484)..controls (341,488) and (325,494)..(316,504)
		..controls (300,537) and (261,574)..(232,540)
		..controls (209,503) and (241,469)..(276,471)
		..controls (299,473) and (344,478)..(353,484)
		--cycle;
	fill (702,500)..controls (714,503) and (731,509)..(739,520)
		..controls (756,553) and (795,591)..(825,557)
		..controls (848,519) and (816,485)..(780,486)
		..controls (757,488) and (711,493)..(702,500)
		--cycle;
	fill (290,571)..controls (279,580) and (262,592)..(246,594)
		..controls (203,588) and (140,602)..(154,654)
		..controls (176,701) and (231,693)..(253,656)
		..controls (266,632) and (291,584)..(290,571)
		--cycle;
	fill (761,591)..controls (772,601) and (789,613)..(805,614)
		..controls (848,609) and (911,623)..(897,674)
		..controls (875,721) and (820,713)..(798,676)
		..controls (785,652) and (760,604)..(761,591)
		--cycle;
);

def p_waterdrip_SBE(expr pos, theta, sc, al) =
	T:=identity aligned al rotated theta scaled sc shifted pos;
	thdraw SBE_water_drip_raw shifted (-(llcorner SBE_water_drip_raw)-(urcorner SBE_water_drip_raw - llcorner SBE_water_drip_raw)/2) scaled (u / max((xpart urcorner SBE_water_drip_raw) - (xpart llcorner SBE_water_drip_raw), (ypart urcorner SBE_water_drip_raw) - (ypart llcorner SBE_water_drip_raw)));
enddef;


def p_wheelchair_SBE(expr pos, theta, sc, al) =
	T:=identity aligned al rotated theta scaled sc shifted pos;
	thdraw SBE_wheelchair_raw shifted (-(llcorner SBE_wheelchair_raw)-(urcorner SBE_wheelchair_raw - llcorner SBE_wheelchair_raw)/2) scaled (u / max((xpart urcorner SBE_wheelchair_raw) - (xpart llcorner SBE_wheelchair_raw), (ypart urcorner SBE_wheelchair_raw) - (ypart llcorner SBE_wheelchair_raw)));
enddef;


%%%%%%%%%%%%%%%%%%%%%%%%% Lines %%%%%%%%%%%%%%%%%%%%%%%%%


def l_abyssentrance_SBE(expr P) =
	path myTick, myT;

	T := identity;

	cursor := 0;
	totalLength := arclength P;
	elementSize := adjust_step(totalLength, 0.5u); % symbol length nudged to be multiple of path length
	myTick := (0,0.1u) -- (0,-0.1u);
	myT := (0,0) -- (0,-0.1u);
	pickup PenD;
	forever:
		t1 := arctime (cursor + elementSize * 1/8) of P;
		t2 := arctime (cursor + elementSize * 2/8) of P;
		t3 := arctime (cursor + elementSize * 4/8) of P;
		t4 := arctime (cursor + elementSize * 6/8) of P;
		t5 := arctime (cursor + elementSize * 7/8) of P;

		thdraw myTick rotated angle(thdir(P,t1)) shifted (point t1 of P);
		thdraw (subpath (t2,t4) of P);
		thdraw myT rotated (180+angle(thdir(P,t3))) shifted (point t3 of P);
		thdraw myTick rotated angle(thdir(P,t5)) shifted (point t5 of P);

		cursor := cursor + elementSize;
		exitif cursor > totalLength - (elementSize / 2);
	endfor;
enddef;


def l_dripline_SBE(expr P) =
	path myTick;

	T := identity;

	cursor := 0;
	totalLength := arclength P;
	elementSize := adjust_step(totalLength, 0.5u); % symbol length nudged to be multiple of path length
	myTick := (0,0.1u) -- (0,-0.1u);
	pickup PenD;
	forever:
		t1 := arctime (cursor + elementSize * 1/8) of P;
		t2 := arctime (cursor + elementSize * 2/8) of P;
		t3 := arctime (cursor + elementSize * 4/8) of P;
		t4 := arctime (cursor + elementSize * 6/8) of P;
		t5 := arctime (cursor + elementSize * 7/8) of P;

		thdraw myTick rotated angle(thdir(P,t1)) shifted (point t1 of P);
		thdraw (subpath (t2,t4) of P);
		thdraw myTick rotated angle(thdir(P,t5)) shifted (point t5 of P);

		cursor := cursor + elementSize;
		exitif cursor > totalLength - (elementSize / 2);
	endfor;
enddef;

