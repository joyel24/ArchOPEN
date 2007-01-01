<html>
<body>
<H1>Irc log for #archopen</h1>

<?

if (isset($_GET["disp_log"]))
{
$disp_log=$_GET["disp_log"];
echo "<a href='ao_ircLog.php'>Back</a><br><br>\n";
     $string_text=file_get_contents("irclog/".$disp_log); // load text file in var
     
     $new_text=htmlspecialchars($string_text);
	$new_text=nl2br($new_text); // convert CR & LF in <br> in newvar
     echo $new_text; // print out HTML formatted text
     unset($string_text, $new_text); // clear all vars to unload memory

echo "<br><br><a href='ao_ircLog.php'>Back</a><br><br>\n";
}
else
{

echo "<a href='/tiki-index.php'>back</a><p>\n";

echo "<ul>\n";

$time_inc=time();

$first_day=mkTime(0,0,0,08,26,2006);

/* creating table of date + including files and mark presence*/

while($time_inc>=$first_day)
{
    $fname="archopen." . date("Ymd",$time_inc). ".log";
    
    if(file_exists("irclog/".$fname))
        echo "<li><a href='ao_ircLog.php?disp_log=".$fname."'>".date("d/m/Y",$time_inc)."</a>\n";
    $time_inc-=24*60*60;
}
echo "</ul>\n";
echo "<p><a href='/tiki-index.php'>back</a>\n";
}
?>

</body>
</html>