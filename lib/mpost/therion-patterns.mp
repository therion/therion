% This file is part of Therion package
% Copyright (C) 1999-2000 Martin Budaj

beginpattern(LakePattern);
    draw origin--10up withpen pensquare scaled (0.2 * Defaultthickness);
    patternxstep(Interline);
    patterntransform(identity rotated -45);
endpattern;
