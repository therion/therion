package main;

# This package does following:
# 1. Traverses entire file structure
# 2. Fills hash thSURVEYS and gives ID number to each survey
# 3. Process all the data according to the thCOMMANDS hash

print "main.\n\n";

# Traverse entire file structure and create a summary file named th__data.th

print STDOUT "Traversing input file structure...";

open 'thDATAFILE', '>th__data.th';

foreach $file (@ARGV) {
        appendfile ($file, 'fh000');
        }

sub appendfile {                            # appends file to summary file

    local($filename, $input) = @_;
    $filename =~ /(.*\/)/;                  # gets file's directory
    local $currentdir = $1;
    $input++;

    unless (open $input, $filename) {
           print STDERR "Couldn't open $filename: $!\n";
           return;
           };

    # chdir for later file attachement
    print thDATAFILE "\\chdir $currentdir\n";    

    while (<$input>) {

          if (/^\s*\\include (.*)/) {       # search the include file
             appendfile ($currentdir . $1, $input);
             print thDATAFILE "\\chdir $currentdir\n";
             }

             else {                         # write line to summary file
                  s/(\\\s*$)//;             # join lines ended with '\'
                  print thDATAFILE;
                  };
             };
    };                                      # end of appendfile

close thDATAFILE;

print STDOUT "done.\n";



# Scan the data file for surveys and give them ID

print STDOUT "Scaning survey structure...";

open thDATAFILE, 'th__data.th';

$default_name = 'survey0000';
$current_survey = '';
$survey_id=0;

while (<thDATAFILE>) {
      if (/^\s*\\survey\s*(\w*)/) {   # \survey found
         $survey_name = $1;
         unless (length($survey_name)) {$survey_name = ++$default_name;};
         if (length($current_survey)) {$survey_name .= '.'};
         $current_survey = $survey_name . $current_survey;
         unless (defined $thSURVEYS{$current_survey}) {
                $thSURVEYS{$current_survey} = ++$survey_id;
                };
         };
      if (/^\s*\\endsurvey/) {                 # \endsurvey found
         $current_survey =~ /\.(.+)$/;
         $current_survey = $1;
         };
      };

close thDATAFILE;

print STDOUT "done.\n";



# process all data

print STDOUT "Processing data...";

open thDATAFILE, 'th__data.th';

$default_name = 'survey0000';
$current_survey = '';
@current_data = ();
$current_sub = '';
$current_prefix = '';

sub process_current_data {
    if ((length($current_sub) > 0) && (@current_data > 0)
       && ($thCSURVEYID > 0)) {
       eval "$current_sub(\@current_data)";
       };
    $current_sub = '';
    $current_prefix = '';
    @current_data = ();
    };

LINE:
while (<thDATAFILE>) {

      next LINE if (/^\s*#/);

      if (/^\s*\\survey\s*(\w*)/) {   # \survey found
         process_current_data;
         $survey_name = $1;
         unless (length($survey_name)) {$survey_name = ++$default_name;};
         if (length($current_survey)) {$survey_name .= '.'};
         $current_survey = $survey_name . $current_survey;
         $thCSURVEYNAME = $current_survey;
         $thCSURVEYID = $thSURVEYS{$thCSURVEYNAME};
         next LINE;
         };

      if (/^\s*\\endsurvey/) {                 # \endsurvey found
         process_current_data;
         $current_survey =~ /\.(.+)$/;
         $current_survey = $1;
         $thCSURVEYNAME = $current_survey;
         $thCSURVEYID = $thSURVEYS{$thCSURVEYNAME};
         next LINE;
         };

      if (/^\s*\\chdir\s*(.*)$/) {
         $thCSURVEYDIR = $1;
         next LINE;
         }

      $read_line = $_;
      $read_line =~ /^\s*\\(\w+)/;
      $read_type = $1;

      # if we're in definition block of some object.
      if (length($current_prefix) > 0) {
         if ($read_type eq $current_prefix) {
            push @current_data, $read_line;
            process_current_data;
            next LINE;
            }
         }

      elsif (defined $thCOMMANDS{$read_type}) {
         process_current_data;
         $current_sub = $thCOMMANDS{$read_type};
         $current_prefix = 'end' . $read_type;
         };

      push @current_data, $read_line;
      };

close thDATAFILE;

print STDOUT "done.\n";



# This sub returns a normalized format of point's name

sub export_norm_point {
    my $outpoint = shift;

    if ($outpoint =~ /^(\S+)@(\S*)$/) {

       my $pname = $1;
       my $sname = $2;

       if (length($thCSURVEYNAME) > 0) {
          $sname .= '.' . $thCSURVEYNAME;
          };

       if (defined($thSURVEYS{$sname})) {
          $outpoint = $pname . '@' . $thSURVEYS{$sname};
          } else { $outpoint =~ s/\@/\./; $outpoint .= '@' . $thCSURVEYID};

       return $outpoint;
       }

    else {
         $outpoint .= '@' . $thCSURVEYID;
         return $outpoint;
         };
    };

sub define_transform {
    package defined;
    do $main::thCSURVEYDIR . @_[0];
    package main;
    };

sub update_csurveydir {
    $thCSURVEYDIR = @_[0];
    };


# end of main

__END__


Global symbols used in this script:

thDATAFILE - file handle for summary file
%thCOMMANDS - hash of known global commands (definition indicators)
%thSURVEYS  - hash of all surveys with their ID
$thCSURVEYNAME  - name of current survey
$thCSURVEYID - id of current survey
$thCSURVEYDIR - directory of current survey

