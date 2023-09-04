<style type="text/css">
ul.none {list-style-type:none}

#framecontentC1{
position: absolute; 
top: 0; 
left: 0%; 
width: 100%;
height: auto;
overflow: visible;
background-color: white;
color: black;
}

</style>

</head>

<body text="#000000" bgcolor="#ffffff" link="#000000" vlink="#000000" alink="000000">
<font size="5" face="helvetica" color="#000000">

<div id="framecontentC1">
<?php
date_default_timezone_set("Pacific/Auckland");
echo "<h4>The time is " . date("H:i:s") . "</h4>";
// read text file
$lines = file('book1.txt');
$dcount = count($lines);
for($y = 0; $y<$dcount; $y=$y+2)
{
	$link = $lines[$y+1];
	///echo "XXX \"$lines[$y]\"";
	if (strpos( $lines[$y], 'Title') != FALSE)
	{
		echo "</ul>";
		echo "<h4>$link</h4>\n";
		echo "<ul class=\"none\">";
	}
	else
	{
		echo "<li><A HREF=\" $link \" >$lines[$y]</A></li><br>\n";
	}
}
?>
</div>
</font>
</body>
