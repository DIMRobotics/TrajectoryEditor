#!/usr/bin/perl

$input_code = $ARGV[0];
$input_report = $ARGV[1];

$_ = $input_code;
if (!(/.*\.c/)) {
        print "Usage: cerebellum_update.pl code.c report.txt\n";
        exit;
}

$_ = $input_report;
if (!(/.*\.txt/)) {
        print "Usage: cerebellum_update.pl code.c report.txt\n";
        exit;
}

$output_code = "/tmp/cerebelum_fgen.c";

$radius = 141;

$res_num = 480;
$res_denom = 22;

open (INPUT_CODE, "<".$input_code) or die "Can't open input file $input_code: $!";
open (INPUT_REPORT, "<".$input_report) or die "Can't open input file $input_report: $!";
open (OUTPUT, ">".$output_code) or die "Can't open output file $output_code: $!";

$set_coords = 0;
$start_angle = 0;

$path = 0;
$angle = 0;

$speed = 100;
$acc_num = 1;
$acc_denom = 7;

$rspeed = 100;
$racc_num = 1;
$racc_denom = 7;

$line = 0;

while ($report = <INPUT_REPORT>) {
        
        if ($set_coords != 0) {
                ($path, $angle) = split /\s+/, $report;
        }
        
        # skip code without markdown
        $last = 0;
        while (<INPUT_CODE>) {
                $line++;
                if (/\/\*\!.*/) {
                        $last = $_;
                        last;
                } else {
                        print OUTPUT $_;
                }
        }

        if (!<INPUT_CODE>) {
                print "[INFO] End of input code; attach extra actions from file\n";
        }
        
        $_ = $last;

        if (/\/\*\!.*/) {
                s/\/\*\!//;
                s/ \*\///;
                chomp;

                # get field value
                if (/res_num:.*/) {
                        s/res_num: //;
                        $res_num = $_;
                        print "[CODE] read res_num: $res_num\n";
                        print OUTPUT "/*!res_num: $res_num */\n";
                        redo;
                } elsif (/res_den:.*/) {
                        s/res_den: //;
                        $res_denom = $_;
                        print "[CODE] read res_den: $res_denom\n";
                        print OUTPUT "/*!res_den: $res_denom */\n";
                        redo;
                } elsif (/radius:.*/) {
                        s/radius: //;
                        $radius = $_;
                        print "[CODE] read radius: $radius\n";
                        print OUTPUT "/*!radius: $radius */\n";
                        redo;
                } elsif (/rspeed:.*/) {
                        s/rspeed: //;
                        $rspeed = $_;
                        print "[CODE] read rspeed: $rspeed\n";
                        print OUTPUT "/*!rspeed: $rspeed */\n";
                        redo;
                } elsif (/racc_num:.*/) {
                        s/racc_num: //;
                        $racc_num = $_;
                        print "[CODE] read racc_num: $racc_num\n";
                        print OUTPUT "/*!racc_num: $racc_num */\n";
                        redo;
                } elsif (/racc_den:.*/) {
                        s/racc_den: //;
                        $racc_denom = $_;
                        print "[CODE] read racc_den: $racc_denom\n";
                        print OUTPUT "/*!racc_den: $racc_denom */\n";
                        redo;
                } elsif (/speed:.*/) {
                        s/speed: //;
                        $speed = $_;
                        print "[CODE] read speed: $speed\n";
                        print OUTPUT "/*!speed: $speed */\n";
                        redo;
                } elsif (/acc_num:.*/) {
                        s/acc_num: //;
                        $acc_num = $_;
                        print "[CODE] read acc_num: $acc_num\n";
                        print OUTPUT "/*!acc_num: $acc_num */\n";
                        redo;
                } elsif (/acc_den:.*/) {
                        s/acc_den: //;
                        $acc_denom = $_;
                        print "[CODE] read acc_den: $acc_denom\n";
                        print OUTPUT "/*!acc_den: $acc_denom */\n";
                        redo;
                } elsif (/coords:.*/) {
                        if ($set_coords > 0) {
                                print "[ERROR] Redefinition of start coordinates!\n";
                                exit;
                        }

                        $set_coords = 1;
                        ($coord_x, $coord_y, $start_angle) = split /\s+/, $report;

                        if (!$start_angle) {
                                $set_coords = 2;
                        }
                        
                        print "[UPDATE] Update start coords at line $line\n";
                        print OUTPUT "/*!coords: $coord_x $coord_y */\n";
                        next;
                } elsif (/angle:.*/) {
                        if ($set_coords == 1) {
                                $set_coords = 2;
                                $angle = $start_angle - (3.141592 / 2);
                                print "[CODE] Read angle from coords block\n";
                        }
                 
                        print "[UPDATE] Update angle at line $line\n";
                        print OUTPUT "/*!angle: $angle */\n";
                        if ($angle > 0) {
                                printf OUTPUT "chassis_move($rspeed, -$rspeed, $racc_num, $racc_denom, %.0f);\n", $angle * $radius * $res_num / $res_denom;
                        } else {
                                printf OUTPUT "chassis_move(-$rspeed, $rspeed, $racc_num, $racc_denom, %.0f);\n", -$angle * $radius * $res_num / $res_denom;
                        }
                        next;
                } elsif (/path:.*/) {
                        print "[UPDATE] Update path at line $line\n";
                        print OUTPUT "/*!path: $path */\n";
                        printf OUTPUT "chassis_move($speed, $speed, $acc_num, $acc_denom, %.0f);\n", $path * $res_num / $res_denom;

                        next;
                }
                
                print $_;

                print "\n";
        } 
}

while (<INPUT_CODE>) {
        print OUTPUT $_;
}

close OUTPUT;
close INPUT_CODE;

rename $output_code, $input_code; # move result
