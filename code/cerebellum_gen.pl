#!/usr/bin/perl

$input = $ARGV[0];
$output = $ARGV[1];

$_ = $output;
if (!(/.*\.c/)) {
        print "Usage: cerebellum_gen.pl report.txt code.c\n";
        exit;
}

$_ = $input;
if (!(/.*\.txt/)) {
        print "Usage: cerebellum_gen.pl report.txt code.c\n";
        exit;
}

$speed = 100;
$acc_num = 1;
$acc_denom = 7;

$rspeed = 100;
$racc_num = 1;
$racc_denom = 7;
$radius = 141;

$res_num = 480;
$res_denom = 22;

# Start of code

open (INPUT, $input) or die ("Cannot open report file $input: $!");
open (OUTPUT, ">".$output) or die ("Cannot open result file $output: $!");

# parse 1st string in file
my ($start_x, $start_y, $start_angle) = split /\s+/, <INPUT>;

if (!$start_x || !$start_y) {
        print "Hey, wrong report file syntax! (cannot read start points)";
}

# print some extra information
print OUTPUT "/* robot description: */\n";
print OUTPUT "/*!res_num: $res_num */\n";
print OUTPUT "/*!res_den: $res_denom */\n";
print OUTPUT "/*!radius: $radius */\n";
print OUTPUT "/*!speed: $speed */\n";
print OUTPUT "/*!acc_num: $acc_num */\n";
print OUTPUT "/*!acc_den: $acc_denom */\n";
print OUTPUT "/*!rspeed: $rspeed */\n";
print OUTPUT "/*!racc_num: $racc_num */\n";
print OUTPUT "/*!racc_den: $racc_denom */\n\n";


# place robot at start point
print OUTPUT "/* place robot at start point */\n";
print OUTPUT "/* note that robot must be placed at OX direction */\n";
print OUTPUT "/*!coords: $start_x $start_y */\n";
#print OUTPUT "chassis_write(-20, -20);\n";

print OUTPUT "\n";

if (!$start_angle) {
        print "Okay, you want just to place robot at the starting point";
        exit;
}

print OUTPUT "/* set start angle */\n";
print OUTPUT "/*!angle: $start_angle */\n";

if ($start_angle > 0) {
        printf OUTPUT "chassis_move($rspeed, -$rspeed, $racc_num, $racc_denom, %.0f); /* mmToTicks(%.6f) aka rotate to %.2f degrees (%.6f radians) */\n", $start_angle * $radius * $res_num / $res_denom, $start_angle * $radius, $start_angle * 180 / 3.141592, $start_angle;
} else {
        printf OUTPUT "chassis_move(-$rspeed, $rspeed, $racc_num, $racc_denom, %.0f); /* mmToTicks(%.6f) aka rotate to %.2f degrees(%.6f radians) */\n", -$start_angle * $radius * $res_num / $res_denom, -$start_angle * $radius, $start_angle * 180 / 3.141592, $start_angle;
}

print OUTPUT "while (chassis_busy()) {\n\t/* do something */\n}\n\n";

$steps = 1;

# read whole file
while (<INPUT>) {
        my ($path, $angle) = split /\s+/, $_;

        if ($path) {
                print OUTPUT "/* step $steps: go $path mm */\n";
                print OUTPUT "/*!path: $path */\n";
                printf OUTPUT "chassis_move($speed, $speed, $acc_num, $acc_denom, %.0f);\n", $path * $res_num / $res_denom;
                print OUTPUT "while (chassis_busy()) { \n\t/* do something */\n}\n";
        }

        if ($angle) {
                printf OUTPUT "/* rotate to %.2f degrees (%.6f radians) */\n", $angle * 180 / 3.141592, $angle;
                print OUTPUT "/*!angle: $angle */\n";
                
                if ($angle > 0) {
                        printf OUTPUT "chassis_move($rspeed, -$rspeed, $racc_num, $racc_denom, %.0f);\n", $angle * $radius * $res_num / $res_denom;
                } else {
                        printf OUTPUT "chassis_move(-$rspeed, $rspeed, $racc_num, $racc_denom, %.0f);\n", -$angle * $radius * $res_num / $res_denom;
                }

                print OUTPUT "while (chassis_busy()) {\n\t/*do something */\n}\n";
        }

        print OUTPUT "\n";

        $steps++;
}

