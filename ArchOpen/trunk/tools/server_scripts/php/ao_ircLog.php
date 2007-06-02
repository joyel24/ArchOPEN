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

echo "<a href='http://www.archopen.org/tiki-index.php'>back</a><p>\n";

$time_inc=time();

$first_day=mktime(0,0,0,8,01,06);

/* creating table of date + including files and mark presence*/
$prevInc=$time_inc-24*60*60;
$nxtYear=1;
$val=getdate($time_inc);
echo "<p><b>".$val["year"]."</b><br><table>";
echo "<tr><td width=100><i>". $val["month"]."</i>:</td>";

while($time_inc>=$first_day)
{
    $fname="archopen." . date("Ymd",$time_inc). ".log";
    $val=getdate($time_inc);
    $prevVal=getdate($prevInc);
    //echo $val["mday"]."<br>";
    if($val["mon"]==1)
        $nxtYear=1;
     if($val["mon"]==12 && $nxtYear==1)
    {
        $nxtYear=0;
        echo "</tr></table><p><b>".$val["year"]."</b><br><table>";
    }
    if($prevVal["mday"]==1)
    {
        if($val["mon"]!=12)
            echo "</tr>";
        echo "<tr><td width=100><i>". $val["month"]."</i>:</td>";
    }
    if(file_exists("irclog/".$fname))
        echo "<td width=10><center><a href='ao_ircLog.php?disp_log=".$fname."'>".$val["mday"]."</center></a></td>";
    else
        echo "<td width=10><center>".$val["mday"]."</center></td>";
    //echo date("Ymd",$time_inc) . " ";
    /*if(file_exists("irclog/".$fname))
        echo "<a href='ao_ircLog.php?disp_log=".$fname."'>".date("Ymd",$time_inc)."</a><br>";    
        */
        
    $prevInc=$time_inc;
    $time_inc-=24*60*60;
}
echo "</tr></table><p><a href='http://www.archopen.org/tiki-index.php'>back</a>\n";
}
?>

</body>
</html>