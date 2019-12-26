% Symbol set of Espeleo Grupo de Brasília, author Rodrigo Severo


%%%%%%%%%%%%%%%%%%%%%%%%% Points %%%%%%%%%%%%%%%%%%%%%%%%%


picture EGB_altar_raw;
EGB_altar_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (145,152)..controls (119,155) and (104,187)..(119,209)
		..controls (128,227) and (149,228)..(167,227)
		..controls (189,228) and (211,228)..(232,228)
		..controls (211,247) and (181,265)..(178,296)
		..controls (175,317) and (182,337)..(194,354)
		..controls (224,401) and (253,450)..(284,496)
		..controls (298,516) and (327,514)..(343,498)
		..controls (373,477) and (384,441)..(401,410)
		..controls (421,422) and (438,439)..(460,448)
		..controls (483,454) and (505,427)..(494,406)
		..controls (482,387) and (460,378)..(443,364)
		..controls (427,354) and (412,338)..(394,334)
		..controls (369,329) and (359,355)..(351,373)
		..controls (347,388) and (341,390)..(336,374)
		..controls (320,343) and (303,313)..(287,282)
		..controls (314,258) and (342,237)..(368,212)
		..controls (385,190) and (370,154)..(342,152)
		..controls (277,151) and (211,151)..(145,152)
		--cycle;
	fill (397,500)..controls (364,507) and (345,547)..(361,577)
		..controls (375,609) and (421,619)..(446,595)
		..controls (474,573) and (471,525)..(440,507)
		..controls (428,499) and (412,497)..(397,500)
		--cycle;
	endgroup;
);
picture EGB_altar;
EGB_altar := EGB_altar_raw shifted (-(llcorner EGB_altar_raw)-(urcorner EGB_altar_raw - llcorner EGB_altar_raw)/2) scaled (u / max((xpart urcorner EGB_altar_raw) - (xpart llcorner EGB_altar_raw), (ypart urcorner EGB_altar_raw) - (ypart llcorner EGB_altar_raw)));

def p_altar_EGB(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw EGB_altar;
enddef;


picture EGB_archeo_excavation_raw;
EGB_archeo_excavation_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (287,416)--(292,416)..controls (292,416) and (299,406)..(300,392)
		..controls (300,379) and (292,368)..(292,368)
		..controls (292,368) and (281,375)..(263,375)
		..controls (251,375) and (243,368)..(243,368)
		..controls (243,397) and (262,416)..(287,416)
		--cycle withcolor black;
	fill (416,408)..controls (415,417) and (412,424)..(412,424)
		--(425,431)..controls (435,436) and (444,424)..(444,424)
		..controls (443,398) and (434,393)..(412,392)
		..controls (412,392) and (418,397)..(416,408)
		--cycle withcolor black;
	fill (335,420)..controls (334,431) and (340,440)..(340,440)
		..controls (369,440) and (379,426)..(379,400)
		..controls (379,400) and (363,404)..(355,404)
		..controls (350,404) and (340,400)..(340,400)
		..controls (340,400) and (336,413)..(335,420)
		--cycle withcolor black;
	fill (199,415)--(242,555)--(312,574)
		--(329,558)--(351,580)--(335,597)
		--(422,683)--(466,640)--(379,553)
		--(362,569)--(340,547)--(359,528)
		--(339,457)--cycle withcolor black;
	fill (139,472)..controls (139,472) and (162,469)..(171,469)
		..controls (181,469) and (194,472)..(194,472)
		..controls (189,464) and (187,464)..(194,456)
		..controls (194,456) and (181,454)..(171,454)
		..controls (162,454) and (139,456)..(139,456)
		..controls (133,463) and (129,464)..(139,472)
		--cycle withcolor black;
	fill (361,472)..controls (361,472) and (380,471)..(392,470)
		..controls (409,469) and (426,468)..(447,469)
		..controls (469,471) and (492,472)..(492,472)
		..controls (498,465) and (502,464)..(492,456)
		..controls (492,456) and (469,455)..(447,454)
		..controls (426,452) and (409,453)..(392,454)
		..controls (380,455) and (361,456)..(361,456)
		..controls (366,465) and (365,463)..(361,472)
		--cycle withcolor black;
	endgroup;
);
picture EGB_archeo_excavation;
EGB_archeo_excavation := EGB_archeo_excavation_raw shifted (-(llcorner EGB_archeo_excavation_raw)-(urcorner EGB_archeo_excavation_raw - llcorner EGB_archeo_excavation_raw)/2) scaled (u / max((xpart urcorner EGB_archeo_excavation_raw) - (xpart llcorner EGB_archeo_excavation_raw), (ypart urcorner EGB_archeo_excavation_raw) - (ypart llcorner EGB_archeo_excavation_raw)));

def p_archeoexcavation_EGB(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw EGB_archeo_excavation;
enddef;


picture EGB_audio_raw;
EGB_audio_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (314,130)--(133,283)--(-27,284)
		..controls (-48,290) and (-45,312)..(-45,329)
		--(-45,511)..controls (-45,526) and (-42,529)..(-30,529)
		--(137,531)--(310,679)..controls (327,694) and (345,668)..(345,648)
		--(346,149)..controls (346,135) and (325,121)..(314,130)
		--cycle withcolor black;
	fill (543,131)..controls (515,145) and (528,182)..(546,198)
		..controls (581,242) and (598,298)..(607,353)
		..controls (621,455) and (592,563)..(525,642)
		..controls (516,670) and (555,686)..(574,667)
		..controls (594,648) and (601,619)..(616,596)
		..controls (690,452) and (673,262)..(565,138)
		..controls (560,132) and (551,129)..(543,131)
		--cycle withcolor black;
	fill (474,198)..controls (459,202) and (454,220)..(459,233)
		..controls (464,249) and (478,259)..(484,274)
		..controls (532,365) and (526,483)..(465,566)
		..controls (448,581) and (460,614)..(483,613)
		..controls (502,611) and (509,589)..(520,576)
		..controls (565,503) and (578,411)..(558,327)
		..controls (547,281) and (527,237)..(496,202)
		..controls (490,197) and (481,197)..(474,198)
		--cycle withcolor black;
	fill (402,271)..controls (380,278) and (380,306)..(395,320)
		..controls (424,374) and (423,443)..(391,496)
		..controls (384,505) and (380,519)..(388,529)
		..controls (397,542) and (417,544)..(427,532)
		..controls (440,516) and (448,497)..(455,478)
		..controls (478,412) and (469,335)..(427,278)
		..controls (421,271) and (411,269)..(402,271)
		--cycle withcolor black;
	endgroup;
);
picture EGB_audio;
EGB_audio := EGB_audio_raw shifted (-(llcorner EGB_audio_raw)-(urcorner EGB_audio_raw - llcorner EGB_audio_raw)/2) scaled (u / max((xpart urcorner EGB_audio_raw) - (xpart llcorner EGB_audio_raw), (ypart urcorner EGB_audio_raw) - (ypart llcorner EGB_audio_raw)));

def p_audio_EGB(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw EGB_audio;
enddef;


picture EGB_bat_raw;
EGB_bat_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (277,424)..controls (272,434) and (263,443)..(251,442)
		..controls (248,443) and (244,441)..(241,441)
		..controls (237,449) and (232,458)..(223,461)
		..controls (219,462) and (212,461)..(211,464)
		..controls (218,472) and (227,478)..(237,481)
		..controls (243,486) and (244,476)..(249,474)
		..controls (254,471) and (261,469)..(267,468)
		..controls (271,469) and (269,482)..(271,485)
		..controls (272,483) and (273,480)..(275,478)
		..controls (278,478) and (282,478)..(286,478)
		..controls (287,481) and (288,483)..(289,486)
		..controls (292,481) and (289,472)..(291,469)
		..controls (294,465) and (300,469)..(305,470)
		..controls (311,472) and (315,477)..(319,482)
		..controls (324,484) and (330,478)..(334,476)
		..controls (340,473) and (345,468)..(350,463)
		..controls (344,462) and (337,463)..(332,459)
		..controls (326,454) and (322,448)..(319,442)
		..controls (311,443) and (302,444)..(295,439)
		..controls (289,435) and (284,428)..(281,422)
		..controls (280,416) and (278,423)..(277,424)
		--cycle withcolor black;
	endgroup;
);
picture EGB_bat;
EGB_bat := EGB_bat_raw shifted (-(llcorner EGB_bat_raw)-(urcorner EGB_bat_raw - llcorner EGB_bat_raw)/2) scaled (u / max((xpart urcorner EGB_bat_raw) - (xpart llcorner EGB_bat_raw), (ypart urcorner EGB_bat_raw) - (ypart llcorner EGB_bat_raw)));

def p_bat_EGB(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw EGB_bat;
enddef;


picture EGB_bone_raw;
EGB_bone_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (431,355)..controls (439,357) and (446,363)..(448,371)
		..controls (451,379) and (454,387)..(460,393)
		..controls (465,399) and (466,407)..(461,413)
		..controls (454,420) and (443,422)..(434,420)
		..controls (413,418) and (394,426)..(376,435)
		..controls (359,443) and (343,453)..(326,462)
		..controls (315,458) and (305,452)..(295,446)
		..controls (328,427) and (360,405)..(391,382)
		..controls (401,375) and (405,362)..(416,355)
		..controls (421,353) and (426,354)..(431,355)
		--cycle withcolor black;
	fill (152,355)..controls (166,364) and (173,381)..(188,389)
		..controls (245,431) and (307,467)..(371,498)
		..controls (388,507) and (406,505)..(424,503)
		..controls (440,501) and (458,510)..(462,526)
		..controls (470,539) and (458,559)..(442,552)
		..controls (423,550) and (431,580)..(412,580)
		..controls (393,581) and (385,561)..(374,548)
		..controls (353,523) and (322,509)..(294,492)
		..controls (256,470) and (219,447)..(179,428)
		..controls (163,421) and (146,419)..(129,421)
		..controls (113,423) and (96,403)..(112,391)
		..controls (121,379) and (122,357)..(141,354)
		..controls (144,354) and (149,353)..(152,355)
		--cycle withcolor black;
	fill (260,483)..controls (263,486) and (273,489)..(272,493)
		..controls (253,505) and (233,516)..(215,529)
		..controls (200,541) and (189,556)..(179,570)
		..controls (172,578) and (160,583)..(150,578)
		..controls (141,572) and (144,558)..(135,553)
		..controls (127,549) and (118,557)..(112,550)
		..controls (105,545) and (104,536)..(107,528)
		..controls (110,516) and (120,505)..(133,504)
		..controls (148,501) and (164,507)..(179,504)
		..controls (197,502) and (212,491)..(228,484)
		..controls (234,481) and (240,476)..(246,476)
		..controls (251,478) and (255,481)..(260,483)
		--cycle withcolor black;
	endgroup;
);
picture EGB_bone;
EGB_bone := EGB_bone_raw shifted (-(llcorner EGB_bone_raw)-(urcorner EGB_bone_raw - llcorner EGB_bone_raw)/2) scaled (u / max((xpart urcorner EGB_bone_raw) - (xpart llcorner EGB_bone_raw), (ypart urcorner EGB_bone_raw) - (ypart llcorner EGB_bone_raw)));

def p_bones_EGB(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw EGB_bone;
enddef;


def p_curtains_EGB(expr pos, theta, sc, al) =
	U:=(.3u, .8u);
	pickup PenC;
	for i=-1 upto 1:
		T:=identity rotated theta aligned al scaled (sc * 0.7) shifted (pos + (i * .3u, abs(i) * .15u));
		thdraw (-.15u,.4u)--(0,.15u)--(.15u,.4u);
		thdraw (0,.15u)--(0,0){dir 180}..(-.12u,-.11u)..{dir 0}(0,-.22u)--(0,-.4u);
	endfor;
enddef;


picture EGB_danger_raw;
EGB_danger_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (296,396)
		..controls (295,393) and (295,392)..(294,390)
		..controls (290,386) and (291,386)..(242,386)
		..controls (199,386) and (196,386)..(193,388)..controls (190,389) and (187,394)..(187,398)
		..controls (188,400) and (189,405)..(191,408)
		..controls (197,419) and (230,482)..(233,486)
		..controls (234,487) and (236,488)..(238,489)
		..controls (242,490) and (246,490)..(249,486)
		..controls (251,484) and (270,450)..(293,405)
		..controls (296,401) and (296,400)..(296,396)
		--cycle withcolor red;
	fill (244,419)..controls (245,424) and (249,452)..(249,453)
		..controls (249,455) and (246,459)..(244,459)
		..controls (241,460) and (237,458)..(235,456)
		..controls (235,455) and (234,453)..(235,450)
		..controls (235,444) and (240,419)..(240,418)
		..controls (242,417) and (243,417)..(244,419)
		--cycle withcolor white;
	fill (246,399)..controls (250,403) and (247,410)..(242,410)
		..controls (238,410) and (235,407)..(235,403)
		..controls (235,398) and (243,395)..(246,399)
		--cycle withcolor white;
	endgroup;
);
picture EGB_danger;
EGB_danger := EGB_danger_raw shifted (-(llcorner EGB_danger_raw)-(urcorner EGB_danger_raw - llcorner EGB_danger_raw)/2) scaled (u / max((xpart urcorner EGB_danger_raw) - (xpart llcorner EGB_danger_raw), (ypart urcorner EGB_danger_raw) - (ypart llcorner EGB_danger_raw)));

def p_danger_EGB(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw EGB_danger;
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


picture EGB_electric_light_raw;
EGB_electric_light_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (312,246)--(338,246)--(338,178)
		--(312,178)--cycle;
	fill (218,315)--(233,294)--(159,238)
		--(144,259)--cycle;
	fill (255,273)--(278,260)--(255,219)
		--(233,232)--cycle;
	fill (365,254)--(388,267)--(412,227)
		--(389,214)--cycle;
	fill (415,293)--(430,313)--(497,264)
		--(481,243)--cycle;
	fill (334,564)..controls (341,561) and (346,556)..(349,550)
		..controls (350,548) and (351,546)..(351,543)
		..controls (349,543) and (347,543)..(345,543)
		..controls (329,543) and (313,543)..(296,543)
		..controls (294,545) and (296,548)..(297,550)
		..controls (300,556) and (305,561)..(312,564)
		..controls (317,567) and (324,567)..(330,565)
		..controls (332,565) and (333,565)..(334,564)
		--cycle;
	fill (364,463)..controls (366,436) and (387,416)..(400,393)
		..controls (410,379) and (415,361)..(411,344)
		..controls (406,312) and (380,287)..(350,280)
		..controls (316,271) and (278,283)..(256,310)
		..controls (238,333) and (236,367)..(251,391)
		..controls (264,414) and (284,433)..(288,460)
		..controls (287,468) and (296,466)..(301,466)
		..controls (322,466) and (343,466)..(364,466)
		..controls (364,465) and (364,464)..(364,463)
		--cycle;
	fill (375,501)..controls (381,499) and (383,492)..(381,487)
		..controls (379,482) and (374,478)..(369,478)
		..controls (350,477) and (332,478)..(313,478)
		..controls (301,478) and (289,477)..(277,478)
		..controls (270,481) and (268,491)..(274,496)
		..controls (277,500) and (281,502)..(286,501)
		..controls (315,501) and (343,501)..(372,501)
		..controls (373,501) and (374,501)..(375,501)
		--cycle;
	fill (377,534)..controls (381,532) and (383,526)..(381,522)
		..controls (380,518) and (377,514)..(372,513)
		..controls (361,512) and (350,512)..(339,512)
		..controls (320,512) and (301,512)..(283,513)
		..controls (279,513) and (274,513)..(272,517)
		..controls (269,521) and (270,527)..(273,531)
		..controls (276,536) and (282,535)..(287,536)
		..controls (306,536) and (326,536)..(346,536)
		..controls (355,536) and (365,536)..(374,536)
		..controls (375,535) and (376,535)..(377,534)
		--cycle;
	endgroup;
);
picture EGB_electric_light;
EGB_electric_light := EGB_electric_light_raw shifted (-(llcorner EGB_electric_light_raw)-(urcorner EGB_electric_light_raw - llcorner EGB_electric_light_raw)/2) scaled (u / max((xpart urcorner EGB_electric_light_raw) - (xpart llcorner EGB_electric_light_raw), (ypart urcorner EGB_electric_light_raw) - (ypart llcorner EGB_electric_light_raw)));

def p_electriclight_EGB(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw EGB_electric_light;
enddef;


picture EGB_exvoto_raw;
EGB_exvoto_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (144,285)..controls (142,285) and (138,289)..(128,298)
	    ..controls (118,309) and (116,311)..(115,314)
	    ..controls (114,319) and (118,326)..(125,331)
	    ..controls (130,334) and (136,334)..(140,330)
	    ..controls (142,329) and (142,329)..(173,360)
	    --(204,391)--(207,398)..controls (212,413) and (222,435)..(231,450)
	    --(234,457)--(254,439)--(246,423)
	    ..controls (242,415) and (238,408)..(238,407)
	    ..controls (238,407) and (239,407)..(241,408)
	    ..controls (244,409) and (264,419)..(269,422)
	    ..controls (271,423) and (272,423)..(273,422)
	    ..controls (274,421) and (279,418)..(285,414)
	    --(294,407)--(284,401)..controls (267,391) and (250,383)..(232,376)
	    --(223,373)--(191,342)--(160,310)
	    --(162,308)..controls (163,307) and (164,305)..(164,303)
	    ..controls (165,299) and (164,296)..(159,291)
	    ..controls (153,284) and (149,283)..(144,285)
	    --cycle;
	fill (410,391)--(441,360)..controls (472,329) and (472,329)..(474,330)
	    ..controls (476,332) and (479,333)..(482,333)
	    ..controls (487,333) and (490,331)..(495,325)
	    ..controls (498,322) and (499,318)..(499,314)
	    ..controls (498,311) and (496,309)..(485,298)
	    ..controls (471,283) and (469,282)..(463,284)..controls (459,285) and (451,295)..(450,298)
	    ..controls (449,302) and (450,306)..(452,308)
	    --(454,310)--(423,342)--(391,373)
	    --(382,376)..controls (341,392) and (303,415)..(269,444)
	    ..controls (244,465) and (214,498)..(191,528)
	    --(185,537)--(180,533)..controls (172,526) and (167,526)..(159,533)
	    ..controls (157,536) and (154,539)..(154,540)
	    ..controls (153,543) and (153,547)..(154,550)
	    ..controls (155,551) and (173,569)..(194,590)
	    ..controls (231,627) and (233,629)..(236,630)
	    ..controls (241,630) and (245,628)..(250,623)
	    ..controls (257,616) and (257,610)..(250,603)
	    ..controls (248,601) and (246,599)..(246,598)
	    ..controls (246,598) and (250,595)..(254,592)
	    ..controls (281,572) and (310,546)..(329,524)
	    ..controls (363,488) and (390,444)..(407,400)
	    --(410,391)--(370,420)..controls (350,461) and (325,494)..(290,528)
	    ..controls (272,545) and (250,563)..(235,574)
	    --(227,580)--(215,568)--(203,556)
	    --(208,548)..controls (221,531) and (241,507)..(260,488)
	    ..controls (282,465) and (311,442)..(339,426)
	    ..controls (348,420) and (375,406)..(376,407)
	    ..controls (376,407) and (373,413)..(370,420)
	    --cycle;
	fill (380,482)..controls (380,483) and (376,488)..(373,493)
	    --(367,501)--(369,504)..controls (370,505) and (376,511)..(381,517)
	    ..controls (389,527) and (402,543)..(409,552)
	    --(411,556)--(387,580)--(381,575)
	    ..controls (371,568) and (361,560)..(350,551)
	    ..controls (344,546) and (339,541)..(338,540)
	    --(336,538)--(326,547)--(317,557)
	    --(321,560)..controls (329,567) and (347,582)..(356,589)
	    ..controls (361,593) and (366,596)..(367,597)
	    ..controls (368,598) and (368,598)..(364,603)
	    ..controls (357,610) and (357,615)..(364,623)
	    ..controls (369,628) and (373,630)..(378,630)
	    ..controls (381,629) and (383,627)..(420,590)
	    ..controls (441,569) and (459,551)..(460,550)
	    ..controls (460,549) and (461,546)..(461,544)
	    ..controls (461,540) and (459,536)..(453,532)
	    ..controls (446,526) and (441,526)..(434,533)
	    --(430,537)--(426,532)..controls (416,518) and (384,480)..(382,480)
	    ..controls (382,480) and (381,481)..(380,482)
	    --cycle;
	endgroup;
);
picture EGB_exvoto;
EGB_exvoto := EGB_exvoto_raw shifted (-(llcorner EGB_exvoto_raw)-(urcorner EGB_exvoto_raw - llcorner EGB_exvoto_raw)/2) scaled (u / max((xpart urcorner EGB_exvoto_raw) - (xpart llcorner EGB_exvoto_raw), (ypart urcorner EGB_exvoto_raw) - (ypart llcorner EGB_exvoto_raw)));

def p_exvoto_EGB(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw EGB_exvoto;
enddef;


picture EGB_gate_raw;
EGB_gate_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (338,237)--(339,587)..controls (403,573) and (458,520)..(483,475)
		..controls (487,469) and (490,462)..(493,455)
		--(492,209)--cycle;
	fill (109,455)--(112,461)..controls (113,464) and (116,471)..(119,475)
		..controls (147,527) and (187,563)..(233,579)
		..controls (241,582) and (253,585)..(258,586)
		..controls (260,586) and (262,586)..(263,587)
		--(264,436)--(282,436)--(282,372)
		--(264,372)--(264,237)--(262,236)
		--(110,209)--(109,209)--cycle;
	fill (138,293)--(170,299)--(170,240)
		--(138,235)--cycle withcolor white;
	fill (198,304)--(231,310)--(231,251)
		--(198,246)--cycle withcolor white;
	fill (138,460)..controls (147,481) and (158,495)..(170,506)
		--(170,330)--(138,324)--cycle withcolor white;
	fill (198,526)..controls (206,537) and (221,547)..(231,552)
		--(231,342)--(198,337)--cycle withcolor white;
	fill (460,293)--(427,299)--(427,240)
		--(460,235)--cycle withcolor white;
	fill (399,304)--(367,310)--(367,251)
		--(399,246)--cycle withcolor white;
	fill (460,460)..controls (451,481) and (440,495)..(427,506)
		--(427,330)--(460,324)--cycle withcolor white;
	fill (399,526)..controls (391,537) and (376,547)..(367,552)
		--(367,342)--(399,337)--cycle withcolor white;
	endgroup;
);
picture EGB_gate;
EGB_gate := EGB_gate_raw shifted (-(llcorner EGB_gate_raw)-(urcorner EGB_gate_raw - llcorner EGB_gate_raw)/2) scaled (u / max((xpart urcorner EGB_gate_raw) - (xpart llcorner EGB_gate_raw), (ypart urcorner EGB_gate_raw) - (ypart llcorner EGB_gate_raw)));

def p_gate_EGB(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw EGB_gate;
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


picture EGB_human_bone_raw;
EGB_human_bone_raw := image(
	% Bottom left bone
	fill (185,328)..controls (169,332) and (162,349)..(171,362)
		..controls (174,366) and (174,366)..(170,366)
		..controls (156,366) and (145,384)..(151,398)
		..controls (159,415) and (183,415)..(192,399)
		..controls (193,397) and (193,396)..(194,395)
		..controls (194,395) and (206,401)..(221,408)
		..controls (236,415) and (249,421)..(250,421)
		..controls (252,422) and (253,420)..(254,414)
		..controls (255,404) and (260,400)..(277,396)
		..controls (279,395) and (277,394)..(244,379)
		--(209,362)--(211,359)..controls (218,342) and (202,323)..(185,328)
		--cycle withcolor black;

	% Bottom right bone
	fill (417,328)..controls (404,332) and (397,347)..(402,359)
		--(404,362)--(369,378)..controls (350,387) and (334,395)..(334,395)
		..controls (333,395) and (335,396)..(338,396)
		..controls (351,399) and (359,405)..(359,414)
		..controls (359,418) and (360,421)..(362,421)
		..controls (362,421) and (375,416)..(390,409)
		..controls (405,402) and (418,396)..(419,396)
		..controls (419,395) and (420,397)..(421,398)
		..controls (428,415) and (453,415)..(462,398)
		..controls (468,384) and (458,366)..(443,366)
		..controls (439,366) and (439,366)..(442,362)
		..controls (451,347) and (442,329)..(425,327)
		..controls (422,327) and (419,327)..(417,328)
		--cycle withcolor black;

	% Face
	fill (291,405)..controls (272,406) and (264,409)..(264,415)
		..controls (264,425) and (260,430)..(245,436)
		..controls (230,443) and (229,444)..(228,451)
		..controls (227,455) and (226,458)..(225,460)
		..controls (222,465) and (223,466)..(228,472)
		..controls (233,479) and (233,478)..(231,495)
		..controls (224,539) and (225,559)..(238,578)
		..controls (265,617) and (347,618)..(374,579)
		..controls (388,560) and (390,540)..(382,495)
		..controls (379,479) and (379,479)..(385,472)
		..controls (390,466) and (391,465)..(388,460)
		..controls (386,458) and (386,455)..(385,451)
		..controls (384,444) and (383,443)..(368,437)
		..controls (353,430) and (348,425)..(348,415)
		..controls (348,408) and (338,405)..(311,405)
		..controls (302,405) and (293,405)..(291,405)
		--cycle withcolor black;

	% Nose
	fill (323,450)..controls (323,456) and (312,480)..(307,480)
		..controls (303,481) and (290,457)..(291,450)
		..controls (296,446) and (317,445)..(323,450)
		--cycle withcolor white;

	% Left eye
	fill (274,470)..controls (286,472) and (293,482)..(293,496)
		..controls (293,508) and (287,514)..(272,516)
		..controls (259,518) and (252,514)..(248,500)
		..controls (244,488) and (244,475)..(249,472)
		..controls (253,470) and (265,469)..(274,470)
		--cycle withcolor white;

	% Right eye
	fill (363,472)..controls (367,474) and (368,477)..(368,486)
		..controls (367,502) and (361,514)..(352,516)
		..controls (343,518) and (327,514)..(323,508)
		..controls (314,495) and (323,473)..(338,470)
		..controls (344,469) and (360,470)..(363,472)
		--cycle withcolor white;

	% Top left bone
	fill (167,445)..controls (142,450) and (144,486)..(169,489)
		--(173,490)--(172,492)..controls (164,504) and (167,519)..(179,526)
		..controls (198,535) and (219,516)..(211,497)
		--(209,493)--(215,490)..controls (223,487) and (224,484)..(219,478)
		..controls (212,470) and (211,465)..(215,457)
		..controls (219,449) and (219,449)..(206,455)
		..controls (200,457) and (194,460)..(194,460)
		..controls (194,460) and (193,458)..(192,456)
		..controls (188,448) and (177,443)..(167,445)
		--cycle withcolor black;

	% Top right bone
	fill (436,445)..controls (429,447) and (423,452)..(421,457)
		..controls (419,460) and (420,461)..(407,455)
		..controls (401,452) and (397,450)..(396,450)
		..controls (395,451) and (396,453)..(398,457)
		..controls (402,465) and (401,470)..(394,478)
		..controls (389,484) and (389,486)..(398,490)
		--(404,493)--(402,497)..controls (396,512) and (407,529)..(424,528)
		..controls (443,527) and (452,508)..(441,492)
		--(440,490)--(444,489)..controls (463,487) and (471,464)..(457,451)
		..controls (451,445) and (443,443)..(436,445)
		--cycle withcolor black;
);
picture EGB_human_bone;
EGB_human_bone := EGB_human_bone_raw shifted (-(llcorner EGB_human_bone_raw)-(urcorner EGB_human_bone_raw - llcorner EGB_human_bone_raw)/2) scaled (u / max((xpart urcorner EGB_human_bone_raw) - (xpart llcorner EGB_human_bone_raw), (ypart urcorner EGB_human_bone_raw) - (ypart llcorner EGB_human_bone_raw)));

def p_humanbones_EGB(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw EGB_human_bone;
enddef;


picture EGB_masonry_raw;
EGB_masonry_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (247,458)--(317,458)--(317,426)
		--(247,426)--cycle;
	fill (286,416)--(358,416)--(358,385)
		--(286,385)--cycle;
	fill (206,416)--(277,416)--(277,385)
		--(206,385)--cycle;
	fill (327,375)--(397,375)--(397,344)
		--(327,344)--cycle;
	fill (247,375)--(317,375)--(317,344)
		--(247,344)--cycle;
	fill (327,458)--(397,458)--(397,426)
		--(327,426)--cycle;
	fill (286,499)--(358,499)--(358,467)
		--(286,467)--cycle;
	endgroup;
);
picture EGB_masonry;
EGB_masonry := EGB_masonry_raw shifted (-(llcorner EGB_masonry_raw)-(urcorner EGB_masonry_raw - llcorner EGB_masonry_raw)/2) scaled (u / max((xpart urcorner EGB_masonry_raw) - (xpart llcorner EGB_masonry_raw), (ypart urcorner EGB_masonry_raw) - (ypart llcorner EGB_masonry_raw)));

def p_masonry_EGB(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw EGB_masonry;
enddef;


picture UIS_minus_raw;
UIS_minus_raw := image(
	fill (294,411)--(307,411)--(307,407)
		--(294,407)--cycle;
	pickup pencircle scaled 1.3bp;
	draw (314,409)..controls (314,402) and (308,396)..(301,396)
		..controls (293,396) and (287,402)..(287,409)
		..controls (287,417) and (293,423)..(301,423)
		..controls (308,423) and (314,417)..(314,409)
		--cycle withcolor black;
);
picture UIS_minus;
UIS_minus := UIS_minus_raw shifted (-(llcorner UIS_minus_raw)-(urcorner UIS_minus_raw - llcorner UIS_minus_raw)/2) scaled (u / max((xpart urcorner UIS_minus_raw) - (xpart llcorner UIS_minus_raw), (ypart urcorner UIS_minus_raw) - (ypart llcorner UIS_minus_raw)));

def p_minus_UIS(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw UIS_minus;
enddef;


picture EGB_mud_raw;
EGB_mud_raw := image(
	fill (240,321)..controls (212,328) and (226,361)..(222,381)
	    ..controls (214,404) and (184,402)..(166,394)
	    ..controls (151,385) and (135,374)..(117,378)
	    ..controls (91,380) and (69,403)..(72,429)
	    ..controls (67,451) and (77,465)..(92,479)
	    ..controls (90,499) and (44,499)..(58,524)
	    ..controls (72,536) and (115,563)..(80,576)
	    ..controls (63,584) and (43,597)..(43,618)
	    ..controls (48,642) and (79,647)..(99,637)
	    ..controls (118,634) and (129,611)..(148,612)
	    ..controls (176,625) and (137,659)..(162,670)
	    ..controls (186,680) and (209,660)..(233,668)
	    ..controls (248,682) and (265,702)..(286,681)
	    ..controls (302,664) and (328,667)..(339,687)
	    ..controls (350,709) and (375,706)..(385,685)
	    ..controls (408,667) and (412,628)..(441,620)
	    ..controls (465,616) and (493,646)..(514,623)
	    ..controls (528,605) and (508,576)..(537,566)
	    ..controls (558,545) and (515,524)..(524,498)
	    ..controls (527,481) and (548,446)..(514,454)
	    ..controls (499,459) and (487,447)..(506,439)
	    ..controls (521,427) and (543,403)..(523,386)
	    ..controls (497,372) and (466,369)..(437,376)
	    ..controls (412,372) and (404,324)..(373,344)
	    ..controls (356,354) and (336,348)..(334,327)
	    ..controls (312,312) and (284,345)..(317,354)
	    ..controls (299,370) and (293,329)..(277,323)
	    ..controls (266,317) and (252,318)..(240,321)
	    --cycle withcolor (0, 0, 0);
);
picture EGB_mud;
EGB_mud := EGB_mud_raw shifted (-(llcorner EGB_mud_raw)-(urcorner EGB_mud_raw - llcorner EGB_mud_raw)/2) scaled (u / max((xpart urcorner EGB_mud_raw) - (xpart llcorner EGB_mud_raw), (ypart urcorner EGB_mud_raw) - (ypart llcorner EGB_mud_raw)));

def p_mud_EGB(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw EGB_mud;
enddef;


picture EGB_mud_crack_raw;
EGB_mud_crack_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (320,501)..controls (322,501) and (328,493)..(334,481)
		..controls (337,474) and (340,469)..(340,468)
		..controls (339,468) and (335,467)..(330,465)
		..controls (324,464) and (320,463)..(320,462)
		..controls (320,462) and (320,462)..(320,462)
		--(320,457)..controls (321,457) and (323,458)..(324,458)
		..controls (329,459) and (334,460)..(336,460)
		..controls (339,461) and (343,461)..(345,462)
		..controls (347,462) and (348,462)..(350,462)
		..controls (352,462) and (354,461)..(360,460)
		..controls (369,457) and (371,456)..(375,456)
		..controls (377,455) and (379,454)..(379,454)
		..controls (379,454) and (381,453)..(383,453)
		..controls (387,451) and (387,450)..(384,447)
		..controls (382,445) and (381,443)..(380,443)
		..controls (380,442) and (377,440)..(375,438)
		..controls (373,436) and (371,434)..(371,434)
		..controls (370,434) and (368,433)..(366,431)
		..controls (364,430) and (359,427)..(356,426)
		..controls (346,421) and (347,421)..(342,424)
		..controls (336,427) and (329,428)..(320,427)
		..controls (320,427) and (320,427)..(320,427)
		--(320,423)..controls (324,424) and (331,424)..(331,424)
		..controls (331,423) and (327,407)..(323,395)
		..controls (322,390) and (321,387)..(320,385)
		--(325,385)..controls (326,389) and (329,397)..(333,415)
		..controls (335,420) and (336,422)..(336,422)
		..controls (337,422) and (339,421)..(342,420)
		..controls (347,417) and (346,418)..(348,404)
		..controls (349,396) and (350,390)..(351,385)
		--(355,385)..controls (355,385) and (355,385)..(355,385)
		..controls (355,386) and (354,390)..(353,392)
		..controls (351,401) and (349,410)..(349,414)
		..controls (348,418) and (348,419)..(350,420)
		..controls (359,423) and (370,428)..(372,428)
		..controls (373,428) and (375,427)..(376,425)
		..controls (378,424) and (382,420)..(386,417)
		..controls (389,414) and (394,409)..(397,407)
		--(401,403)--(399,399)..controls (398,396) and (396,393)..(395,392)
		..controls (394,391) and (393,389)..(393,388)
		..controls (393,387) and (394,386)..(395,385)
		--(397,385)--(396,387)--(397,391)
		..controls (398,393) and (399,397)..(400,399)
		--(403,402)--(409,401)..controls (412,401) and (416,400)..(416,400)
		..controls (417,399) and (420,397)..(423,394)
		..controls (427,390) and (429,389)..(429,390)
		..controls (429,391) and (427,394)..(423,397)
		--(417,404)--(410,404)--(403,404)
		--(399,408)..controls (397,410) and (392,415)..(388,419)
		..controls (385,423) and (382,427)..(381,427)
		..controls (381,427) and (380,429)..(380,430)
		..controls (378,433) and (378,433)..(387,442)
		--(393,448)--(395,447)..controls (396,446) and (398,445)..(400,443)
		..controls (401,442) and (403,440)..(404,440)
		..controls (405,439) and (408,436)..(411,434)
		..controls (417,428) and (425,420)..(429,417)
		--(431,415)--(436,415)--(436,419)
		--(433,419)--(427,424)..controls (424,427) and (421,430)..(421,430)
		..controls (421,431) and (417,434)..(413,438)
		..controls (408,442) and (404,446)..(402,448)
		..controls (400,449) and (399,450)..(398,450)
		..controls (398,450) and (397,451)..(396,452)
		..controls (395,454) and (396,456)..(401,460)
		..controls (404,463) and (405,464)..(406,464)
		..controls (407,464) and (407,464)..(408,464)
		..controls (410,463) and (430,461)..(436,460)
		--(436,464)..controls (429,464) and (409,467)..(408,468)
		..controls (407,469) and (404,480)..(404,483)
		..controls (404,484) and (404,485)..(407,485)
		..controls (408,485) and (413,486)..(417,487)
		..controls (421,487) and (428,488)..(432,489)
		..controls (433,489) and (434,489)..(436,490)
		--(436,493)..controls (433,492) and (431,492)..(429,492)
		..controls (426,491) and (419,490)..(414,489)
		..controls (407,488) and (404,488)..(403,488)
		..controls (403,489) and (403,495)..(403,501)
		--(399,501)..controls (399,491) and (399,486)..(400,483)
		..controls (400,481) and (401,477)..(402,474)
		--(403,468)--(399,464)..controls (397,462) and (394,460)..(393,459)
		..controls (391,458) and (390,457)..(389,457)
		..controls (387,457) and (352,467)..(351,467)
		..controls (351,467) and (355,473)..(357,475)
		..controls (357,475) and (360,479)..(363,483)
		..controls (367,488) and (370,491)..(370,492)
		..controls (371,493) and (375,498)..(378,501)
		--(370,501)..controls (369,500) and (368,499)..(367,497)
		..controls (364,494) and (360,489)..(358,486)
		..controls (355,483) and (351,477)..(349,474)
		--(346,468)--(344,471)..controls (343,472) and (340,476)..(338,479)
		..controls (336,483) and (332,489)..(330,493)
		..controls (327,496) and (326,499)..(324,501)
		--cycle withcolor black;
	endgroup;
);
picture EGB_mud_crack;
EGB_mud_crack := EGB_mud_crack_raw shifted (-(llcorner EGB_mud_crack_raw)-(urcorner EGB_mud_crack_raw - llcorner EGB_mud_crack_raw)/2) scaled (u / max((xpart urcorner EGB_mud_crack_raw) - (xpart llcorner EGB_mud_crack_raw), (ypart urcorner EGB_mud_crack_raw) - (ypart llcorner EGB_mud_crack_raw)));

def p_mudcrack_EGB(expr pos, theta, sc, al) =
    U:=(.5u,.5u);
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw EGB_mud_crack;
enddef;


picture EGB_nameplate_raw;
EGB_nameplate_raw := image(
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
picture EGB_nameplate;
EGB_nameplate := EGB_nameplate_raw shifted (-(llcorner EGB_nameplate_raw)-(urcorner EGB_nameplate_raw - llcorner EGB_nameplate_raw)/2) scaled (u / max((xpart urcorner EGB_nameplate_raw) - (xpart llcorner EGB_nameplate_raw), (ypart urcorner EGB_nameplate_raw) - (ypart llcorner EGB_nameplate_raw)));

def p_nameplate_EGB(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw EGB_nameplate;
enddef;


picture EGB_notwheelchair_raw;
EGB_notwheelchair_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (591,447)..controls (591,295) and (465,172)..(309,172)
		..controls (152,172) and (26,295)..(26,447)
		..controls (26,599) and (152,723)..(309,723)
		..controls (465,723) and (591,599)..(591,447)
		--cycle withcolor white;
	fill (260,282)..controls (248,283) and (243,284)..(236,285)
		..controls (185,298) and (145,340)..(137,390)
		..controls (128,444) and (155,497)..(204,525)
		..controls (218,532) and (225,534)..(231,533)
		..controls (240,531) and (246,523)..(244,514)
		..controls (243,509) and (239,505)..(232,502)
		..controls (203,490) and (183,469)..(174,442)
		..controls (170,431) and (169,423)..(169,411)
		..controls (169,394) and (172,382)..(179,368)
		..controls (193,341) and (218,322)..(249,315)
		..controls (257,313) and (277,313)..(286,314)
		..controls (326,321) and (357,349)..(368,387)
		..controls (370,394) and (372,397)..(376,400)
		..controls (381,403) and (387,403)..(393,400)
		..controls (402,395) and (403,388)..(398,373)
		..controls (395,364) and (389,350)..(385,344)
		..controls (365,312) and (332,290)..(294,283)
		..controls (286,282) and (266,281)..(260,282)
		--cycle;
	fill (467,312)..controls (460,314) and (459,316)..(429,368)
		--(401,417)--(350,417)..controls (294,417) and (295,417)..(288,422)
		..controls (283,425) and (280,430)..(278,435)
		..controls (276,442) and (255,547)..(255,550)
		..controls (255,561) and (262,572)..(273,577)
		..controls (277,579) and (278,579)..(286,579)
		..controls (293,579) and (294,579)..(299,577)
		..controls (305,574) and (311,568)..(313,562)
		..controls (314,560) and (316,553)..(317,547)
		--(319,537)--(388,536)--(391,534)
		..controls (396,531) and (399,527)..(399,521)
		..controls (400,514) and (397,509)..(391,506)
		--(387,504)--(326,504)--(327,502)
		..controls (327,501) and (329,493)..(331,484)
		..controls (333,474) and (334,466)..(334,465)
		--(335,463)--(373,463)--(412,463)
		--(417,461)..controls (419,459) and (422,457)..(424,456)
		..controls (427,453) and (490,342)..(492,337)
		..controls (495,330) and (493,323)..(488,317)
		..controls (482,312) and (475,310)..(467,312)
		--cycle;
	fill (265,590)..controls (252,593) and (241,605)..(238,618)
		..controls (235,631) and (238,644)..(248,654)
		..controls (257,662) and (267,666)..(278,665)
		..controls (291,664) and (304,655)..(309,644)
		..controls (316,629) and (313,612)..(302,600)
		..controls (299,597) and (295,594)..(293,593)
		..controls (286,589) and (273,588)..(265,590)
		--cycle;
	fill (481,197)..controls (482,198) and (488,203)..(494,207)
		..controls (585,279) and (628,396)..(605,510)
		..controls (598,544) and (587,573)..(569,604)
		..controls (551,634) and (533,656)..(508,678)
		..controls (426,752) and (312,775)..(207,741)
		..controls (184,733) and (175,729)..(149,713)
		..controls (122,698) and (114,692)..(96,676)
		..controls (9,596) and (-24,475)..(8,362)
		..controls (16,337) and (24,317)..(39,293)
		..controls (57,262) and (76,239)..(103,216)
		..controls (184,146) and (294,123)..(396,155)
		..controls (419,163) and (430,168)..(455,182)
		..controls (468,189) and (479,196)..(481,197)
		--cycle--(277,213)..controls (233,218) and (193,235)..(157,262)
		..controls (147,270) and (125,292)..(117,302)
		..controls (84,346) and (67,395)..(67,449)
		..controls (67,487) and (75,522)..(92,556)
		..controls (98,567) and (111,588)..(112,588)
		..controls (113,588) and (443,258)..(443,257)
		..controls (443,256) and (422,243)..(411,238)
		..controls (385,224) and (359,216)..(331,213)
		..controls (318,212) and (291,212)..(277,213)
		--cycle--(328,473)--(162,639)--(173,647)
		..controls (195,661) and (226,674)..(251,680)
		..controls (271,684) and (279,685)..(304,685)
		..controls (330,685) and (342,684)..(362,679)
		..controls (387,672) and (413,661)..(434,646)
		..controls (449,637) and (456,631)..(469,618)
		..controls (506,582) and (530,537)..(538,484)
		..controls (541,469) and (541,439)..(539,423)
		..controls (535,385) and (522,350)..(501,318)
		..controls (497,312) and (494,307)..(494,307)
		..controls (494,307) and (419,382)..(328,473)
		--cycle withcolor red;
	endgroup;
);
picture EGB_notwheelchair;
EGB_notwheelchair := EGB_notwheelchair_raw shifted (-(llcorner EGB_notwheelchair_raw)-(urcorner EGB_notwheelchair_raw - llcorner EGB_notwheelchair_raw)/2) scaled (u / max((xpart urcorner EGB_notwheelchair_raw) - (xpart llcorner EGB_notwheelchair_raw), (ypart urcorner EGB_notwheelchair_raw) - (ypart llcorner EGB_notwheelchair_raw)));

def p_nowheelchair_EGB(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw EGB_notwheelchair;
enddef;


picture EGB_pendant_raw;
EGB_pendant_raw := image(
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
picture EGB_pendant;
EGB_pendant := EGB_pendant_raw shifted (-(llcorner EGB_pendant_raw)-(urcorner EGB_pendant_raw - llcorner EGB_pendant_raw)/2) scaled (u / max((xpart urcorner EGB_pendant_raw) - (xpart llcorner EGB_pendant_raw), (ypart urcorner EGB_pendant_raw) - (ypart llcorner EGB_pendant_raw)));

def p_pendant_EGB(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw EGB_pendant;
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


picture EGB_photo_raw;
EGB_photo_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (196,477)..controls (190,477) and (190,477)..(188,475)
	    --(186,472)--(186,445)--(111,445)
	    --(108,443)..controls (103,439) and (75,410)..(74,408)
	    ..controls (73,407) and (73,390)..(73,357)
	    ..controls (73,314) and (73,308)..(74,306)
	    ..controls (75,305) and (76,303)..(77,303)
	    ..controls (78,303) and (102,302)..(132,302)
	    --(185,302)--(186,296)..controls (186,290) and (187,288)..(191,286)
	    --(191,286)..controls (191,286) and (222,286)..(258,286)
	    --(325,286)--(327,288)..controls (329,291) and (329,291)..(329,296)
	    --(329,302)--(436,302)--(439,304)
	    --(441,306)--(441,409)--(424,426)
	    ..controls (415,435) and (406,443)..(405,444)
	    ..controls (404,445) and (401,445)..(366,445)
	    --(329,445)--(329,472)--(327,475)
	    ..controls (325,477) and (324,477)..(319,477)
	    --cycle withcolor black;
	pickup pencircle scaled 14bp;
	interim linecap := rounded;
	interim linejoin := beveled;
	draw (193,500)--(108,620) withcolor black;
	draw (320,500)--(406,620) withcolor black;
	endgroup;
);
picture EGB_photo;
EGB_photo := EGB_photo_raw shifted (-(llcorner EGB_photo_raw)-(urcorner EGB_photo_raw - llcorner EGB_photo_raw)/2) scaled (u / max((xpart urcorner EGB_photo_raw) - (xpart llcorner EGB_photo_raw), (ypart urcorner EGB_photo_raw) - (ypart llcorner EGB_photo_raw)));

def p_photo_EGB(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw EGB_photo;
enddef;


picture UIS_plus_raw;
UIS_plus_raw := image(
	fill (394,411)--(394,403)--(402,403)
		--(402,400)--(394,400)--(394,392)
		--(392,392)--(392,400)--(384,400)
		--(384,403)--(392,403)--(392,411)
		--cycle withcolor black;
	pickup pencircle scaled 0.75bp;
	draw (394,411)--(394,403)--(402,403)
		--(402,400)--(394,400)--(394,392)
		--(392,392)--(392,400)--(384,400)
		--(384,403)--(392,403)--(392,411)
		--cycle withcolor black;
	pickup pencircle scaled 1.3bp;
	draw (407,401)..controls (407,394) and (401,387)..(393,387)
		..controls (385,387) and (379,394)..(379,401)
		..controls (379,409) and (385,415)..(393,415)
		..controls (401,415) and (407,409)..(407,401)
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
	fill (482,451)--(482,443)--(490,443)
		--(490,441)--(482,441)--(482,433)
		--(480,433)--(480,441)--(472,441)
		--(472,443)--(480,443)--(480,451)
		--cycle withcolor black;
	pickup pencircle scaled 0.75bp;
	draw (482,451)--(482,443)--(490,443)
		--(490,441)--(482,441)--(482,433)
		--(480,433)--(480,441)--(472,441)
		--(472,443)--(480,443)--(480,451)
		--cycle withcolor black;
	pickup pencircle scaled 0.75bp;
	fill (475,425)--(488,425)--(488,421)
		--(475,421)--cycle withcolor black;
	pickup pencircle scaled 1.3bp;
	draw (481,412)..controls (473,412) and (467,418)..(467,426)
		--(467,442)..controls (467,450) and (473,456)..(481,456)
		..controls (489,456) and (495,450)..(495,442)
		--(495,426)..controls (495,418) and (489,412)..(481,412)
		--cycle withcolor black;
);
picture UIS_plus_minus;
UIS_plus_minus := UIS_plus_minus_raw shifted (-(llcorner UIS_plus_minus_raw)-(urcorner UIS_plus_minus_raw - llcorner UIS_plus_minus_raw)/2) scaled (u / ((xpart urcorner UIS_plus_minus_raw) - (xpart llcorner UIS_plus_minus_raw)));

def p_plusminus_UIS(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw UIS_plus_minus;
enddef;


picture EGB_seed_germination_raw;
EGB_seed_germination_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (289,648)..controls (283,648) and (276,647)..(269,644)
		..controls (253,636) and (262,641)..(235,620)
		..controls (220,609) and (206,593)..(195,575)
		..controls (186,559) and (182,554)..(175,536)
		..controls (165,511) and (159,482)..(154,454)
		..controls (151,435) and (149,426)..(147,406)
		..controls (143,374) and (140,341)..(136,308)
		..controls (134,287) and (132,265)..(132,244)
		--(153,258)..controls (152,279) and (153,300)..(155,321)
		..controls (158,354) and (160,386)..(164,418)
		..controls (167,438) and (169,447)..(172,466)
		..controls (177,494) and (183,522)..(194,548)
		..controls (201,565) and (205,570)..(213,586)
		..controls (223,600) and (233,613)..(246,624)
		..controls (249,626) and (257,635)..(255,631)
		..controls (253,625) and (235,617)..(252,625)
		..controls (262,630) and (273,630)..(284,630)
		..controls (294,629) and (305,624)..(314,618)
		..controls (319,615) and (321,610)..(321,603)
		..controls (321,600) and (321,597)..(321,594)
		--(342,608)..controls (342,612) and (342,615)..(342,618)
		..controls (341,625) and (338,631)..(333,635)
		..controls (324,642) and (313,646)..(302,647)
		..controls (298,648) and (294,648)..(289,648)
		--cycle;
	fill (349,586)--(321,570)..controls (323,568) and (325,565)..(327,563)
		--(327,563)..controls (325,558) and (325,553)..(324,548)
		..controls (323,545) and (322,544)..(321,542)
		..controls (317,536) and (311,532)..(308,526)
		..controls (304,518) and (302,509)..(298,500)
		..controls (296,494) and (292,488)..(287,483)
		..controls (280,478) and (277,473)..(274,465)
		..controls (273,464) and (273,462)..(272,460)
		--(301,474)..controls (301,475) and (302,477)..(302,478)
		..controls (304,482) and (304,483)..(307,486)
		..controls (310,490) and (313,492)..(316,495)
		..controls (321,500) and (324,507)..(326,513)
		..controls (329,510) and (331,507)..(333,505)
		--(327,502)..controls (328,493) and (327,484)..(327,475)
		..controls (327,468) and (327,461)..(327,454)
		..controls (328,447) and (329,440)..(332,433)
		..controls (334,427) and (337,422)..(341,417)
		..controls (341,415) and (342,413)..(342,411)
		..controls (343,410) and (343,409)..(344,407)
		..controls (348,401) and (353,396)..(359,392)
		..controls (361,390) and (363,388)..(366,386)
		..controls (371,383) and (376,379)..(381,377)
		..controls (384,375) and (387,374)..(391,372)
		--(417,389)..controls (416,390) and (414,391)..(412,392)
		--(416,394)..controls (418,396) and (419,400)..(418,403)
		..controls (418,410) and (417,416)..(415,422)
		..controls (413,429) and (411,436)..(410,443)
		--(409,442)..controls (407,446) and (406,450)..(403,453)
		..controls (399,460) and (387,477)..(383,483)
		..controls (380,487) and (377,492)..(373,496)
		..controls (374,496) and (374,497)..(374,497)
		--(373,497)..controls (365,508) and (356,520)..(349,533)
		..controls (348,535) and (346,538)..(346,540)
		..controls (345,542) and (349,545)..(349,546)
		..controls (350,547) and (350,547)..(351,548)
		..controls (354,546) and (358,545)..(362,544)
		..controls (362,543) and (362,543)..(362,542)
		..controls (364,536) and (370,530)..(374,525)
		..controls (377,522) and (378,517)..(382,515)
		..controls (382,514) and (384,514)..(384,513)
		--(386,514)..controls (387,513) and (387,512)..(387,512)
		..controls (390,507) and (394,501)..(397,497)
		..controls (401,491) and (405,484)..(408,477)
		..controls (409,475) and (409,474)..(410,472)
		--(437,487)..controls (436,489) and (435,490)..(435,492)
		..controls (431,499) and (427,506)..(423,513)
		..controls (420,518) and (417,521)..(414,527)
		..controls (413,529) and (413,531)..(412,533)
		..controls (408,542) and (404,552)..(395,558)
		..controls (386,562) and (376,565)..(367,569)
		..controls (363,571) and (359,574)..(356,577)
		..controls (355,578) and (355,579)..(355,580)
		--(354,579)..controls (352,581) and (350,583)..(349,586)
		--cycle;
	fill (33,233)..controls (42,234) and (51,236)..(59,237)
		..controls (83,238) and (106,239)..(130,242)
		..controls (153,245) and (176,248)..(199,251)
		..controls (216,253) and (233,253)..(250,253)
		..controls (270,253) and (290,257)..(310,256)
		..controls (319,255) and (322,254)..(330,252)
		..controls (333,251) and (336,250)..(338,249)
		..controls (340,248) and (342,247)..(345,246)
		..controls (351,244) and (359,242)..(366,241)
		..controls (377,240) and (392,241)..(404,242)
		..controls (431,243) and (458,245)..(485,247)
		..controls (502,248) and (520,248)..(537,248)
		..controls (539,248) and (541,247)..(544,247)
		..controls (544,247) and (566,265)..(566,265)
		..controls (564,265) and (562,265)..(560,266)
		..controls (542,265) and (525,266)..(507,265)
		..controls (480,263) and (452,259)..(425,258)
		..controls (412,258) and (400,257)..(388,258)
		..controls (375,259) and (364,264)..(353,269)
		..controls (345,271) and (341,272)..(332,273)
		..controls (323,273) and (314,272)..(305,272)
		..controls (287,271) and (290,271)..(271,270)
		..controls (265,270) and (260,270)..(255,270)
		..controls (244,270) and (233,271)..(222,269)
		..controls (198,266) and (174,262)..(151,259)
		..controls (134,257) and (118,256)..(102,255)
		..controls (99,255) and (95,255)..(92,255)
		..controls (88,256) and (87,255)..(82,255)
		..controls (74,254) and (65,253)..(57,252)
		..controls (57,252) and (33,233)..(33,233)
		--cycle;
	endgroup;
);
picture EGB_seed_germination;
EGB_seed_germination := EGB_seed_germination_raw shifted (-(llcorner EGB_seed_germination_raw)-(urcorner EGB_seed_germination_raw - llcorner EGB_seed_germination_raw)/2) scaled (u / max((xpart urcorner EGB_seed_germination_raw) - (xpart llcorner EGB_seed_germination_raw), (ypart urcorner EGB_seed_germination_raw) - (ypart llcorner EGB_seed_germination_raw)));

def p_seedgermination_EGB(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw EGB_seed_germination;
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


picture EGB_tree_trunk_raw;
EGB_tree_trunk_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (223,206)..controls (172,211) and (132,248)..(122,297)
		..controls (114,332) and (124,370)..(148,397)
		--(382,582)..controls (387,585) and (393,578)..(390,574)
		--(155,387)..controls (145,374) and (138,361)..(134,345)
		..controls (132,337) and (132,336)..(131,321)
		..controls (131,305) and (132,299)..(136,288)
		..controls (144,264) and (161,243)..(183,231)
		..controls (192,225) and (207,220)..(219,218)
		..controls (227,216) and (244,217)..(252,218)
		..controls (273,222) and (293,232)..(308,247)
		--(489,476)..controls (493,478) and (498,473)..(496,469)
		--(326,250)..controls (316,239) and (309,231)..(298,224)
		..controls (286,216) and (268,209)..(254,207)
		..controls (245,205) and (230,205)..(223,206)
		--cycle;
	fill (219,237)..controls (192,242) and (168,260)..(157,285)
		..controls (153,296) and (151,302)..(151,315)
		..controls (150,327) and (151,334)..(154,344)
		..controls (159,356) and (165,367)..(174,376)
		..controls (212,415) and (276,406)..(303,360)
		..controls (315,341) and (317,317)..(311,296)
		..controls (303,266) and (278,243)..(248,237)
		..controls (242,236) and (225,236)..(219,237)
		--cycle;
	fill (371,396)--(437,472)..controls (441,474) and (446,469)..(444,465)
		--(377,394)..controls (374,394) and (373,394)..(371,396)
		--cycle;
	fill (281,435)..controls (279,437) and (278,439)..(279,442)
		--(349,511)..controls (352,511) and (354,509)..(355,506)
		--(284,434)..controls (283,434) and (282,435)..(281,435)
		--cycle;
	fill (386,487)..controls (384,488) and (384,489)..(384,491)
		--(454,562)..controls (457,562) and (458,561)..(459,560)
		--cycle;
	endgroup;
);
picture EGB_tree_trunk;
EGB_tree_trunk := EGB_tree_trunk_raw shifted (-(llcorner EGB_tree_trunk_raw)-(urcorner EGB_tree_trunk_raw - llcorner EGB_tree_trunk_raw)/2) scaled (u / max((xpart urcorner EGB_tree_trunk_raw) - (xpart llcorner EGB_tree_trunk_raw), (ypart urcorner EGB_tree_trunk_raw) - (ypart llcorner EGB_tree_trunk_raw)));

def p_treetrunk_EGB(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw EGB_tree_trunk;
enddef;


picture EGB_volcano_raw;
EGB_volcano_raw := image(
	pickup pencircle scaled 21bp;
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	draw (36,222)--(169,567)..controls (177,510) and (212,485)..(265,485)
		..controls (319,485) and (353,510)..(361,567)
		--(494,222);
	endgroup;
);
picture EGB_volcano;
EGB_volcano := EGB_volcano_raw shifted (-(llcorner EGB_volcano_raw)-(urcorner EGB_volcano_raw - llcorner EGB_volcano_raw)/2) scaled (u / max((xpart urcorner EGB_volcano_raw) - (xpart llcorner EGB_volcano_raw), (ypart urcorner EGB_volcano_raw) - (ypart llcorner EGB_volcano_raw)));

def p_volcano_EGB(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw EGB_volcano;
enddef;


picture EGB_walkway_raw;
EGB_walkway_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (100,152)..controls (107,151) and (112,156)..(113,162)
		..controls (113,169) and (108,174)..(102,175)
		..controls (96,175) and (90,170)..(90,164)
		..controls (89,158) and (94,152)..(100,152)
		--cycle;
	fill (91,147)..controls (93,148) and (96,150)..(99,149)
		..controls (103,149) and (106,147)..(108,144)
		--(119,121)--(135,110)..controls (137,109) and (137,107)..(137,105)
		..controls (137,102) and (134,100)..(132,100)
		..controls (131,100) and (130,101)..(129,101)
		--(112,113)..controls (111,113) and (111,114)..(111,115)
		--(106,123)--(101,100)--(122,76)
		..controls (122,75) and (122,75)..(122,74)
		--(128,45)..controls (128,44) and (128,44)..(128,43)
		..controls (128,39) and (124,36)..(120,36)
		..controls (116,36) and (113,39)..(113,42)
		--(107,69)--(91,87)--(87,70)
		..controls (87,69) and (86,67)..(85,66)
		--(70,40)..controls (68,37) and (65,36)..(62,36)
		..controls (58,36) and (55,40)..(55,44)
		..controls (55,45) and (56,47)..(56,48)
		--(71,72)--(83,127)--(75,121)
		--(71,101)..controls (70,99) and (68,97)..(65,97)
		..controls (63,97) and (60,99)..(61,102)
		--(61,103)--(66,126)..controls (66,127) and (67,128)..(67,128)
		--cycle;
	fill (18,65)--(30,65)--(30,56)
		--(18,56)--cycle;
	fill (42,65)--(54,65)--(54,56)
		--(42,56)--cycle;
	fill (89,65)--(101,65)--(101,56)
		--(89,56)--cycle;
	fill (136,65)--(148,65)--(148,56)
		--(136,56)--cycle;
	fill (159,65)--(171,65)--(171,56)
		--(159,56)--cycle;
	fill (18,31)--(30,31)--(30,21)
		--(18,21)--cycle;
	fill (42,31)--(54,31)--(54,21)
		--(42,21)--cycle;
	fill (65,31)--(77,31)--(77,21)
		--(65,21)--cycle;
	fill (113,31)--(125,31)--(125,21)
		--(113,21)--cycle;
	fill (89,31)--(101,31)--(101,21)
		--(89,21)--cycle;
	fill (136,31)--(148,31)--(148,21)
		--(136,21)--cycle;
	fill (159,31)--(171,31)--(171,21)
		--(159,21)--cycle;
	endgroup;
);
picture EGB_walkway;
EGB_walkway := EGB_walkway_raw shifted (-(llcorner EGB_walkway_raw)-(urcorner EGB_walkway_raw - llcorner EGB_walkway_raw)/2) scaled (u / max((xpart urcorner EGB_walkway_raw) - (xpart llcorner EGB_walkway_raw), (ypart urcorner EGB_walkway_raw) - (ypart llcorner EGB_walkway_raw)));

def p_walkway_EGB(expr pos, theta, sc, al) =
	T:=identity rotated theta aligned al scaled sc shifted pos;
	thdraw EGB_walkway;
enddef;


picture EGB_water_drip_raw;
EGB_water_drip_raw := image(
	% Central
	fill (255,242)..controls (228,245) and (206,257)..(187,278)
		..controls (173,292) and (168,305)..(166,324)
		..controls (165,340) and (168,359)..(174,374)
		..controls (178,384) and (192,410)..(217,457)
		..controls (245,509) and (251,523)..(259,547)
		..controls (264,562) and (269,585)..(269,592)
		..controls (269,594) and (269,595)..(270,595)
		..controls (270,595) and (270,594)..(270,592)
		..controls (270,585) and (275,562)..(280,547)
		..controls (288,523) and (294,509)..(322,457)
		..controls (347,411) and (362,381)..(366,372)
		..controls (375,348) and (376,320)..(367,299)
		..controls (364,291) and (358,284)..(348,274)
		..controls (331,256) and (313,246)..(291,243)
		..controls (283,241) and (263,241)..(255,242)
		--cycle withcolor black;

	% Reflex in central
	fill (292,261)..controls (302,263) and (312,267)..(321,273)
		..controls (338,284) and (349,299)..(354,315)
		..controls (357,324) and (357,336)..(354,346)
		..controls (348,365) and (336,391)..(320,415)
		..controls (314,425) and (306,438)..(305,438)
		..controls (305,438) and (307,433)..(310,428)
		..controls (323,401) and (332,375)..(335,353)
		..controls (337,340) and (336,322)..(334,313)
		..controls (331,304) and (327,294)..(322,288)
		..controls (314,276) and (301,266)..(286,261)
		..controls (281,260) and (281,260)..(283,260)
		..controls (284,260) and (288,260)..(292,261)
		--cycle withcolor white;

	% Lower left
	fill (129,262)..controls (119,264) and (112,270)..(111,277)
		..controls (111,282) and (112,285)..(116,289)
		..controls (124,297) and (136,294)..(146,282)
		..controls (152,276) and (158,271)..(165,268)
		--(170,266)--(162,264)..controls (148,261) and (138,260)..(129,262)
		--cycle withcolor black;

	% Lower right
	fill (388,262)..controls (386,262) and (381,263)..(377,264)
		--(369,266)--(374,268)..controls (381,271) and (388,276)..(393,282)
		..controls (400,291) and (409,295)..(416,293)
		..controls (425,291) and (430,280)..(426,272)
		..controls (423,266) and (414,262)..(401,261)
		..controls (396,261) and (391,261)..(388,262)
		--cycle withcolor black;

	% Upper left
	fill (155,298)..controls (148,303) and (134,308)..(113,311)
		..controls (96,313) and (92,315)..(87,320)
		..controls (80,326) and (78,331)..(78,340)
		..controls (78,347) and (80,353)..(85,358)
		..controls (89,362) and (93,364)..(100,366)
		..controls (121,371) and (140,353)..(151,318)
		..controls (154,310) and (157,298)..(157,297)
		..controls (157,297) and (156,298)..(155,298)
		--cycle withcolor black;

	% Upper right
	fill (382,297)..controls (382,299) and (386,312)..(388,318)
		..controls (399,353) and (418,371)..(439,366)
		..controls (446,364) and (450,362)..(454,358)
		..controls (459,353) and (461,347)..(461,340)
		..controls (461,331) and (459,326)..(452,320)
		..controls (447,315) and (443,313)..(426,311)
		..controls (405,308) and (391,303)..(384,298)
		..controls (383,297) and (382,297)..(382,297)
		--cycle withcolor black;
);
picture EGB_water_drip;
EGB_water_drip := EGB_water_drip_raw shifted (-(llcorner EGB_water_drip_raw)-(urcorner EGB_water_drip_raw - llcorner EGB_water_drip_raw)/2) scaled (u / max((xpart urcorner EGB_water_drip_raw) - (xpart llcorner EGB_water_drip_raw), (ypart urcorner EGB_water_drip_raw) - (ypart llcorner EGB_water_drip_raw)));

def p_waterdrip_EGB(expr pos, theta, sc, al) =
	T:=identity aligned al rotated theta scaled sc shifted pos;
	thdraw EGB_water_drip;
enddef;


picture EGB_wheelchair_raw;
EGB_wheelchair_raw := image(
	begingroup;
	interim linecap := butt;
	interim linejoin := mitered;
	fill (260,282)..controls (248,283) and (243,284)..(236,285)
		..controls (185,298) and (145,340)..(137,390)
		..controls (128,444) and (155,497)..(204,525)
		..controls (218,532) and (225,534)..(231,533)
		..controls (240,531) and (246,523)..(244,514)
		..controls (243,509) and (239,505)..(232,502)
		..controls (203,490) and (183,469)..(174,442)
		..controls (170,431) and (169,423)..(169,411)
		..controls (169,394) and (172,382)..(179,368)
		..controls (193,341) and (218,322)..(249,315)
		..controls (257,313) and (277,313)..(286,314)
		..controls (326,321) and (357,349)..(368,387)
		..controls (370,394) and (372,397)..(376,400)
		..controls (381,403) and (387,403)..(393,400)
		..controls (402,395) and (403,388)..(398,373)
		..controls (395,364) and (389,350)..(385,344)
		..controls (365,312) and (332,290)..(294,283)
		..controls (286,282) and (266,281)..(260,282)
		--cycle;
	fill (467,312)..controls (460,314) and (459,316)..(429,368)
		--(401,417)--(350,417)..controls (294,417) and (295,417)..(288,422)
		..controls (283,425) and (280,430)..(278,435)
		..controls (276,442) and (255,547)..(255,550)
		..controls (255,561) and (262,572)..(273,577)
		..controls (277,579) and (278,579)..(286,579)
		..controls (293,579) and (294,579)..(299,577)
		..controls (305,574) and (311,568)..(313,562)
		..controls (314,560) and (316,553)..(317,547)
		--(319,537)--(388,536)--(391,534)
		..controls (396,531) and (399,527)..(399,521)
		..controls (400,514) and (397,509)..(391,506)
		--(387,504)--(326,504)--(327,502)
		..controls (327,501) and (329,493)..(331,484)
		..controls (333,474) and (334,466)..(334,465)
		--(335,463)--(373,463)--(412,463)
		--(417,461)..controls (419,459) and (422,457)..(424,456)
		..controls (427,453) and (490,342)..(492,337)
		..controls (495,330) and (493,323)..(488,317)
		..controls (482,312) and (475,310)..(467,312)
		--cycle;
	fill (265,590)..controls (252,593) and (241,605)..(238,618)
		..controls (235,631) and (238,644)..(248,654)
		..controls (257,662) and (267,666)..(278,665)
		..controls (291,664) and (304,655)..(309,644)
		..controls (316,629) and (313,612)..(302,600)
		..controls (299,597) and (295,594)..(293,593)
		..controls (286,589) and (273,588)..(265,590)
		--cycle;
	endgroup;
);
picture EGB_wheelchair;
EGB_wheelchair := EGB_wheelchair_raw shifted (-(llcorner EGB_wheelchair_raw)-(urcorner EGB_wheelchair_raw - llcorner EGB_wheelchair_raw)/2) scaled (u / max((xpart urcorner EGB_wheelchair_raw) - (xpart llcorner EGB_wheelchair_raw), (ypart urcorner EGB_wheelchair_raw) - (ypart llcorner EGB_wheelchair_raw)));

def p_wheelchair_EGB(expr pos, theta, sc, al) =
	T:=identity aligned al rotated theta scaled sc shifted pos;
	thdraw EGB_wheelchair;
enddef;


%%%%%%%%%%%%%%%%%%%%%%%%% Lines %%%%%%%%%%%%%%%%%%%%%%%%%


def l_abyssentrance_EGB(expr P) =
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


def l_dripline_EGB(expr P) =
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


def l_fault_EGB(expr P) =
	path myDot;
	pair Ppb, Pdb, Pvb, Ppe, Pde, Pve;

	T := identity;

	pickup PenA;
	thdraw P;

	cursor := 0;
	totalLength := arclength P;
	elementSize := adjust_step(totalLength, 0.5u); % symbol length nudged to be multiple of path length
	myDot := (0,-0.05u) -- (0,0.05u);
	pickup PenC;
	distance := 0.2u;
	forever:
		t0 := arctime (cursor + elementSize * 1/8) of P;
		t1 := arctime (cursor + elementSize * 5/8) of P;
		t2 := arctime (cursor + elementSize * 7/8) of P;

		Ppb := (point t0 of P);
		Pdb := unitvector(thdir(P,t0));
		Pvb := Pdb rotated -90;
		Ppe := (point t1 of P);
		Pde := unitvector(thdir(P,t1));
		Pve := Pde rotated -90;

		if cursor > 0:
			thdraw (Ppb - distance * Pvb) -- (Ppe - distance * Pve);
		fi;

		thdraw myDot rotated (angle(thdir(P,t2))+90) shifted (point t2 of P + distance * unitvector(thdir(P,t2) rotated 90));

		cursor := cursor + elementSize;
		exitif cursor > totalLength - (elementSize / 2);
	endfor;
enddef;


def l_joint_EGB(expr P) =
	T := identity;

	cursor := 0;
	totalLength := arclength P;
	elementSize := adjust_step(totalLength, u); % symbol length nudged to be multiple of path length
	pickup PenC;
	forever:
		t0 := arctime (cursor + elementSize * 3/22) of P;
		t1 := arctime (cursor + elementSize * 10/22) of P;
		t2 := arctime (cursor + elementSize * 16/22) of P;
		t3 := arctime (cursor + elementSize * 19/22) of P;

		thdraw (subpath (t0,t1) of P);
		thdraw (subpath (t2,t3) of P);

		cursor := cursor + elementSize;
		exitif cursor > totalLength - (elementSize / 2);
	endfor;
enddef;


def l_lowceiling_EGB(expr P) =
	path myT;

	T := identity;

	cursor := 0;
	totalLength := arclength P;
	elementSize := adjust_step(totalLength, 0.5u); % symbol length nudged to be multiple of path length
	myT := (0,0) -- (0,-0.1u);
	pickup PenD;
	forever:
		t1 := arctime (cursor + elementSize * 1/7) of P;
		t2 := arctime (cursor + elementSize * 3/7) of P;
		t3 := arctime (cursor + elementSize * 4/7) of P;
		t4 := arctime (cursor + elementSize * 6/7) of P;

		thdraw (subpath (t1,t4) of P);
		thdraw myT rotated (180+angle(thdir(P,t2))) shifted (point t2 of P);
		thdraw myT rotated (180+angle(thdir(P,t3))) shifted (point t3 of P);

		cursor := cursor + elementSize;
		exitif cursor > totalLength - (elementSize / 2);
	endfor;
enddef;


def l_pitchimney_EGB(expr P) =
	path myTsmall, myTlarge;

	T := identity;

	cursor := 0;
	totalLength := arclength P;
	elementSize := adjust_step(totalLength, 0.5u); % symbol length nudged to be multiple of path length
	myTsmall := (0,0) -- (0,-0.1u);
	myTlarge := (0,0) -- (0,-0.175u);
	pickup PenD;
	forever:
		t1 := arctime (cursor + elementSize * 1/6) of P;
		t2 := arctime (cursor + elementSize * 2/6) of P;
		t3 := arctime (cursor + elementSize * 3/6) of P;
		t4 := arctime (cursor + elementSize * 4/6) of P;
		t5 := arctime (cursor + elementSize * 5/6) of P;

		thdraw (subpath (t1,t5) of P);

		thdraw myTsmall rotated (180+angle(thdir(P,t2))) shifted (point t2 of P);
		thdraw myTlarge rotated (180+angle(thdir(P,t3))) shifted (point t3 of P);
		thdraw myTsmall rotated (180+angle(thdir(P,t4))) shifted (point t4 of P);

		cursor := cursor + elementSize;
		exitif cursor > totalLength - (elementSize / 2);
	endfor;
enddef;


def l_rimstonedam_EGB(expr P) =
	cas := 0;
	distance := arclength P;
	mystep := adjust_step(distance, 0.9u);
	pickup PenC;
	forever:
		t  := arctime (cas + mystep/2) of P;
		T := identity rotated angle(thdir(P,t)) scaled 0.7 shifted (point t of P);
		thdraw (-.45u,.2u){dir -80}..{dir -50}(-.15u,0){dir 30}..{dir -30}(.15u,.1u){dir -50}..{dir 70}(.45u,.2u);
		cas := cas + mystep;
		exitif cas > distance - (2*mystep/3); % for rounding errors
	endfor;
enddef;


def l_rimstonepool_EGB(expr P) =
	cas := 0;
	distance := arclength P;
	mystep := adjust_step(distance, 0.9u);
	pickup PenC;
	forever:
		t  := arctime (cas + mystep/2) of P;
		T := identity rotated angle(thdir(P,t)) scaled 0.7 shifted (point t of P);
		p := (-.45u,.2u){dir -80}..{dir -50}(-.15u,-.1u){dir 30}..{dir -30}(.15u,.1u){dir -50}..{dir 70}(.45u,.2u);
		thfill buildcycle(p,((.5u,.15u)--(-.5u,.15u)));
		thdraw p;
		cas := cas + mystep;
		exitif cas > distance - (2*mystep/3); % for rounding errors
	endfor;
enddef;


def l_walkway_EGB(expr P) =
	pair tick[], d[], p[];

	T := identity;

	cursor := 0;
	totalLength := arclength P;
	elementSize := adjust_step(totalLength, 0.2u); % symbol length nudged to be multiple of path length
	distance := 0.1u;

	pickup PenD;
	forever:
		t1 := arctime (cursor + elementSize * 1/5) of P;
		t2 := arctime (cursor + elementSize * 4/5) of P;

		p1 := point t1 of P;
		p2 := point t2 of P;

		d1 := unitvector(thdir(P,t1)) * distance;
		d2 := unitvector(thdir(P,t2)) * distance;

		tick1 := p1 + (d1 rotated 90);
		tick2 := p2 + (d2 rotated 90);
		tick3 := p1 + (d1 rotated -90);
		tick4 := p2 + (d2 rotated -90);

		thdraw tick1 -- tick2;
		thdraw tick3 -- tick4;

		cursor := cursor + elementSize;
		exitif cursor > totalLength - (elementSize / 2);
	endfor;
enddef;


%%%%%%%%%%%%%%%%%%%%%%%%% Areas %%%%%%%%%%%%%%%%%%%%%%%%%


picture EGB_a_mudcrack_raw;
EGB_a_mudcrack_raw := image(
	begingroup;
		interim linecap := butt;
		interim linejoin := mitered;
		pickup pencircle scaled 8bp;
		draw (172,466)--(225,425)--(271,440)
			--(323,390)--(346,423)--(414,363)
			--(390,275)--(414,281)--(438,322);
		pickup pencircle scaled 4bp;
		draw (244,252)--(232,272)--(308,293)
			--(257,310)--(266,357)--(291,380)
			--(303,428)--(282,477)--(393,502)
			--(380,518);
		pickup pencircle scaled 6bp;
		draw (172,466)--(244,518);
		pickup pencircle scaled 8bp;
		draw (173,322)--(205,357)--(244,322)
			--(266,357);
		pickup pencircle scaled 8bp;
		draw (380,252)--(364,292)--(380,322)
			--(346,423)--(417,491)--(393,502);
		pickup pencircle scaled 8bp;
		draw (437,466)--(417,491)--(380,392);
	endgroup;
);
picture EGB_a_mudcrack;
EGB_a_mudcrack := EGB_a_mudcrack_raw shifted (-(llcorner EGB_a_mudcrack_raw)-(urcorner EGB_a_mudcrack_raw - llcorner EGB_a_mudcrack_raw)/2) scaled (1.5u / max((xpart urcorner EGB_a_mudcrack_raw) - (xpart llcorner EGB_a_mudcrack_raw), (ypart urcorner EGB_a_mudcrack_raw) - (ypart llcorner EGB_a_mudcrack_raw)));

beginpattern(pattern_mudcrack_EGB);
	draw EGB_a_mudcrack;
endpattern;

def a_mudcrack_EGB(expr Path) =
	T := identity;
	thclean Path;
	thfill Path withpattern pattern_mudcrack_EGB;
enddef;


def pillar_main_EGB(expr P) =
	rgbcolor myOrange;
	pair d[], p[];

	T := identity;

	myOrange := (1,0.6,0);

	pickup PenD;
	thfill P withcolor myOrange;

	cursor := 0;
	totalLength := arclength P;
	elementSize := adjust_step(totalLength, 0.01u); % symbol length nudged to be multiple of path length
	borderWidth := 0.3u;

	pickup PenA;
	forever:
		t1 := arctime (cursor + elementSize / 2) of P;

		p1 := point t1 of P;

		d1 := unitvector(thdir(P,t1)) * borderWidth;

		thdraw p1 -- p1 + (d1 rotated 90) withcolor white;;

		cursor := cursor + elementSize;
		exitif cursor > totalLength - (elementSize / 2);
	endfor;

	pickup PenA;
	thdraw P withcolor myOrange;
enddef;

def a_pillar_EGB(expr P) =
	picture myPillar;

	myPillar := image(pillar_main_EGB(P));

	thdraw myPillar;
enddef;


def a_pillarwithcurtains_EGB(expr P) =
	picture myPillar;
	path myCurtain;

	myPillar := image(pillar_main_EGB(P));

	thdraw myPillar;

	myCurtain := (0,-.4u){dir 180}..(-0.1u,-0.3u)..{dir 0}(0,-0.2u)..(0.1u,-0.1u)..{dir -180}(0,0);
	myCurtain := myCurtain scaled (0.2 / 0.4);
	cursor := 0;
	totalLength := arclength P;
	elementSize := adjust_step(totalLength, 0.5u); % symbol length nudged to be multiple of path length

	pickup PenD;
	forever:
		t1 := arctime (cursor + elementSize / 2) of P;

		thdraw myCurtain rotated (180+angle(thdir(P,t1))) shifted (point t1 of P) withcolor (1,0.6,0);

		cursor := cursor + elementSize;
		exitif cursor > totalLength - (elementSize / 3);
	endfor;
enddef;


def a_stalactite_EGB(expr P) =
	rgbcolor myOrange, myLightOrange;
	T := identity;

	myLightOrange := (1,0.9,0.67);
	myOrange := (1,0.6,0);

	pickup PenA;
	thfill P withcolor myLightOrange;
	thdraw P withcolor myOrange;
enddef;


def a_stalactitestalagmite_EGB(expr P) =
	rgbcolor myOrange, myLightOrange;

	picture tmpPicFill, tmpPicDraw;
	path bboxP, clipFill, clipDraw;

	T := identity;

	myLightOrange := (1,0.9,0.67);
	myOrange := (1,0.6,0);
	bboxP := bbox P;
	clipFill := ulcorner bboxP -- llcorner bboxP -- lrcorner bboxP -- cycle;
	clipDraw := ulcorner bboxP -- urcorner bboxP -- lrcorner bboxP -- cycle;

	pickup PenA;
	thclean P;

	tmpPicFill := image(
		fill P withcolor myOrange;
		draw P withcolor myOrange;
	);
	clip tmpPicFill to clipFill;
	drawoptions();
	draw tmpPicFill;

	tmpPicDraw := image(
		fill P withcolor myLightOrange;
		draw P withcolor myOrange;
	);
	clip tmpPicDraw to clipDraw;
	drawoptions();
	draw tmpPicDraw;
enddef;


def a_stalagmite_EGB(expr P) =
	rgbcolor myOrange;
	T := identity;

	myOrange := (1,0.6,0);

	pickup PenA;
	thfill P withcolor myOrange;
enddef;

