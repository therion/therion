%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This file is part of the MPATTERN package
%
% mpattern.mp: MetaPost macros for pattern defining and filling
%
% Author: Piotr Bolek
% version 0.2: (Sep 25, 1998)
%
% $Id: mpattern.mp,v 1.6 1998/09/25 10:48:56 piotrek Exp $
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

boolean P_used_already_;
% Record all numbers (charcode) of pictures
extra_beginfig := extra_beginfig
   & "numeric Pwritten_[]; Pcharcode_[charcode]=1; P_used_already_:=false;";

% Working variables to store fragments of Pattern definition
string PBBox_, PHead_, PTail_, PPaintProc_, Pname_, Pdefs_;
string Pmatrix_;

PHead_="<< /PaintType 1 /PatternType 1 /TilingType 1 ";
% 11.08.98 PPaintProc_="/PaintProc { begin ";
PPaintProc_="/PaintProc {  pop ";
% 11.08.98 PTail_=" end } >>"; % matrix makepattern";
PTail_="  } >>"; % matrix makepattern";
Pdefs_="";
   
% New line string
string Pnl_; Pnl_=char 13;

% Number of defined patterns
numeric Pnum_; Pnum_=0;

% Split string defining pattern on new line chars
% and use each piece as argument to special
vardef Psplit_and_write_(expr s)=
   save Pfirst_, Plast_;
   Pfirst_:=0; Plast_:=0;
   for i=0 upto length(s):
      if ( substring (i,i+1) of s = Pnl_ ):
	 Plast_:=i;
	 special substring (Pfirst_,Plast_) of s;
	 Pfirst_:=i+1;
      fi;
   endfor;
enddef;

% Find maximum charcode not used for picture yet
vardef Pmax_charcode_=
   save Pn_;
   for i=999 downto 1:
      Pn_:=i;
      exitif unknown Pcharcode_[i];
   endfor;
   Pn_
enddef;

def PmakeBB_=
   PBBox_ := "/BBox [" & decimal Plft_ & " " & decimal Plow_ & " "
   & decimal Prt_ & " " & decimal Pup_ & "]" & Pnl_;
enddef;

def Psteps_=
   PBBox_ := PBBox_ & "/XStep " & decimal
   if unknown PXStep_: (Prt_-Plft_) else: PXStep_ fi & Pnl_
   & "/YStep " & decimal
   if unknown PYStep_:  (Pup_-Plow_) else: PYStep_ fi;
enddef;
   
vardef Pfindbounds_=
%   save Plow_, Plft_, Pup_, Prt_;
%   numeric Plow_, Plft_, Pup_, Prt_;

   Plow_ = ypart (llcorner currentpicture);
   Plft_ = xpart (llcorner currentpicture);
   Pup_ = ypart (urcorner currentpicture);
   Prt_ = xpart (urcorner currentpicture);
   
   PmakeBB_;
enddef;

% Read file in which the body of patterns PaintProc is stored
vardef PReadFile_=
   save Pfile_, Pline_, Pall_;
   string Pfile_, Pline_, Pall_;
   Pline_=Pall_="";
   Pfile_:=jobname & "." & decimal charcode;
   forever:
      Pline_ := readfrom Pfile_;
      exitif Pline_ = EOF;
      if substring(length Pline_-3,length Pline_) of Pline_ = "def":
	 Pdefs_ := Pdefs_ & Pline_ & Pnl_;
      else:
	   if (substring (0,2) of Pline_ = "%%")
	      or (substring (0,8) of Pline_ = "showpage")
	      or (substring (0,4) of Pline_ = "%!PS"):
	   else:
		Pall_:=Pall_ & Pline_ & Pnl_;
	   fi;
      fi;
   endfor;
   Pall_
enddef;

% Join all parts of pattern definition 
vardef PmakePattern_(expr name)=
   save Pread_;
   string Pread_;

   Pread_ = PReadFile_;
   
   % Write string used later to identify pattern
%   batchmode;
%   message "Defining pattern " & name & "->" & decimal (Pnum_*epsilon);
%   errorstopmode;
   "% Pattern:" &
      if (not known PColor_):
          decimal (Pnum_*epsilon)
      else:
          decimal (PColor_)
      fi
      & ":" & name & Pnl_
      & PHead_ & Pnl_
      & PBBox_ & Pnl_
      & Pdefs_
      & PPaintProc_ & Pnl_
      & Pread_ 
      & PTail_ & Pnl_
      & Pmatrix_ & " makepattern" & Pnl_
      & "/" & name & " exch def" & Pnl_
enddef;

% find PatternColor -- color which will be replaced by pattern
vardef Pfindcolor_(expr s)=
   save Pfirst_, Plast_;
   numeric Pfirst_, Plast_;
   Pfirst_=Plast_=0;
   for i=0 upto 255:
      if substring(i,i+1) of s=":":
	 if Pfirst_=0:
	    Pfirst_:=i+1;
	 else:
	      Plast_:=i;
	 fi;
      fi;
      exitif Plast_<>0;
   endfor;
   scantokens (substring (Pfirst_,Plast_) of s)
enddef;

%%%%%%%%%%%%%% User interface macros

% Define BoundingBox of pattern
vardef patternbbox(expr a)(text b)=
%   save Plft_, Plow_, Prt_, Pup_, Pi_, Pz_;
%   numeric Plft_, Plow_, Prt_, Pup_, Pi_, Pz_[];
   save Pi_, Pz_;
   numeric Pi_, Pz_[];
   if pair a:
      Plft_:=min(xpart(a),xpart(b)); Plow_:=min(ypart(a),ypart(b));
      Prt_:=max(xpart(a),xpart(b));  Pup_:=max(ypart(a),ypart(b));
   else:
      Pi_=1;
      for t=b:
	 Pz_[Pi_]=t;
	 Pi_:=Pi_+1;
      endfor;
      Plft_:=min(a,Pz_2);   Plow_:=min(Pz_1,Pz_3);
      Prt_:=max(a,Pz_2); Pup_:=max(Pz_1,Pz_3);
   fi;

   PmakeBB_;
enddef;

def beginpattern(suffix name)=
   numeric PXStep_, PYStep_;
   numeric Plow_, Plft_, Pup_, Prt_;
   numeric PColor_;
   Pmatrix_:=" matrix ";

   % Declare variable in which the pattern definition will be stored
   string name;
   Pname_:=str name;

   Pnum_:=Pnum_+1;
   
   % Use the largest available picture number (charcode)
   % for storing the body od patten PaintProc
   beginfig(Pmax_charcode_);
enddef;

def endpattern=
   if unknown PBBox_:
      Pfindbounds_;
   fi;
   endfig;
   Psteps_;

   begingroup;
   save Punknown_, Ppattern_;
   string Punknown_, Ppattern_;

   Ppattern_=PmakePattern_(Pname_);
   % free used charcode
   Pcharcode_[charcode]:=whatever;
   PBBox_:=Punknown_;
   Pdefs_:="";
   % Assign pattern to string variable
   scantokens(Pname_ & "=Ppattern_;");
   endgroup;
enddef;
   
primarydef p withpattern s=
   hide(
	Pc_:=Pfindcolor_(s);
	% write definition of pattern used in picture
	% but not yet written to output file;
	if unknown Pwritten_[Pc_]:
	   Psplit_and_write_(s);
	   Pwritten_[Pc_]:=1;
	fi;
	if not P_used_already_:
           batchmode;
           message "Pattern:" & decimal charcode;
           errorstopmode;
	   P_used_already_:=true;
	fi;
	)
   p withcolor Pc_*white
enddef;
   
def patterntransform expr t=
   Pmatrix_ := "[ " & decimal xxpart t
              & " " & decimal yxpart t
              & " " & decimal xypart t
              & " " & decimal yypart t
              & " " & decimal xpart t
              & " " & decimal ypart t & " ]";
%   show Pmatrix_;
enddef;

def patternxstep expr t=
   show t;
   PXStep_ = t;
enddef;

def patternystep expr t=
   show t;
   PYStep_ = t;
enddef;

def patternstep text t=
   show t;
   if pair t:
      PXStep_ = xpart t;
      PYStep_ = ypart t;
   else:
      (PXStep_,PYStep_)=t;
   fi;
enddef;

def patterncolor expr t=
   show t;
   PColor_ = t;
enddef;
