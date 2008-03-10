<html>
<body>

<?
//include('phpReport_20060827.log');
$today=time();
$nb_days=10;
/* creating table of date + including files and mark presence*/
for($i=0;$i<$nb_days;$i++)
{
	$str[$i]=date("Ymd",$today-$i*24*60*60);
    $str_disp[$i]=date("d-m-Y",$today-$i*24*60*60);
	$fname="builds/phpReport_".$str[$i].".log";
	if(file_exists($fname))
	{
		include($fname);
		$has_file[$i]=true;
	}
	else
		$has_file[$i]=false;
}

?>
<center>
<a href='http://www.archopen.org/tiki-index.php'>
<img src='http://archopen.org/show_image.php?id=57&nocount=y' border=0></a>
<br>
<hr>
<font size=+5><b>Daily Builds</b></font>
</center>
<DIV ALIGN=right><a href='http://www.archopen.org/tiki-index.php'>Back to index</a></div><p>

<H1> Content of this page</h1>

On this page you will find all the files needed to run mediOS:
<ul>
<li> medios binary
<li> codecs for sound playback
<li> icons for the GUI
<li> config files for the GUI
<li> medios applications (games, calendar, text editor, ...)
</ul>
 
These files are available in 3 types of package:
<ul>
<li><a href='#icons'>Icons and config files</a>
<li><a href='#dist'>Full distribution</a>
<li><a href='#bin'>Binary of medios and sound codecs</a>
</ul>

Each day around midnight (GMT+1) we build all this files using the fresh sources from our svn repository.

<?
$arch_list=array("jbmm","av1xx","av3xx","g4xx","g402","av4xx","pma","av5xx","apps");
$arch_name=array("jbmm"=>"Jbmm","av1xx"=>"Av 1xx","av3xx"=>"Av 3xx","g4xx"=>"Gmini 4XX","g402"=>"Gmini 402","av4xx"=>"Av 4xx","pma"=>"PMA","av5xx"=>"Av 5xx","apps"=>"Med Applications");
$nb_arch=count($arch_list);
?>

<H1><a name='icons'>Config files and Icons</a></H1>
 
The full medios folder with icons and config files :  <a href='builds/iconsCfg.zip'>ZIP</a><br>
Unpack the zip and put the medios folder it contains in the root folder of your device. Make sure you overwrite existing files.
        
<p>Icons for Jbmm are available here: <a href='builds/jbmm_icons.zip'>Jbmm</a>

<H1><a name='dist'>Full distribution<a></H1>

<center>
<table border="0" cellspacing="1">
<tr>

<?
for($i=0;$i<$nb_arch;$i++)
{
    if($arch_list[$i]!="apps")
    {
        echo "<td align='center' bgcolor='lightgray'><font color='gray'><b>";
        if(file_exists("icons/".$arch_list[$i].".png"))
            echo "<img src='icons/".$arch_list[$i].".png'><br>";
        echo $arch_name[$arch_list[$i]]."</b></font></td>\n";
    }
}
echo "</tr><tr>\n";
for($i=0;$i<$nb_arch;$i++)
{
    if($arch_list[$i]!="apps")
    {
        echo "<td bgcolor='#66FF66'>";
        echo "<center><a href='builds/".$arch_list[$i]."_medios_dist.zip'>ZIP</a><center></td>\n";
    }
}
echo "</tr>\n";
?>
</table>
</center>

Unpack the zip file, it contains:
<ul>
<li>medios binary (.bin, .avi or .wav file)
<li>medios folder
</ul>
Put the file and the folder in the root folder of your device. Make sure to replace all existing files if you have
previously instaled medios on your device.



<H1><a name='bin'>Binary of medios and sound codecs</a></H1>

We keep here 10 days of daily builds. You find in each cell 3 links:
<ul>
<li><b>ZIP</b>: this the zip file containing the medios binary and the folder with codecs, put the binary (.bin, .avi or .wav according to the arch) and the medios folder in the root folder of your device
<li><b>Err:#-Warn:#</b>: this tells you if errors of warnings were generated during build, the link will open the error/warning log
<li><b>Make log</b>: this is a link to the full build log
</u><p>

You can also download only the applications designed for mediOS using "Med Applications" column.

<center>
<table border="0" cellspacing="1">
    <tr>
      <td bgcolor='lightgray' width='100'><font size=+1><b><center>Date</center></b></font></td>

<?
for($i=0;$i<$nb_arch;$i++)
{
    echo "<td align='center' bgcolor='lightgray'><font color='gray'><b>";
    if(file_exists("icons/".$arch_list[$i].".png"))
        echo "<img src='icons/".$arch_list[$i].".png'><br>";
    echo $arch_name[$arch_list[$i]]."</b></font></td>\n";
}

echo "</tr>\n";

for($j=0;$j<$nb_days;$j++)
{
	echo "<tr>\n";
	$cur_date=$str[$j];
	echo "<td bgcolor='lightgray' width='100'><b>".$str_disp[$j]."</b></td>\n";
	if($has_file[$j])
	{
            for($i=0;$i<$nb_arch;$i++)
            {
                if(array_key_exists($arch_list[$i],$warning[$cur_date]))
                {
                    if($error[$cur_date][$arch_list[$i]]>0)
                            $bgcolor="#FF0066";
                    else if($warning[$cur_date][$arch_list[$i]]>0)
                            $bgcolor="#FF9966";
                    else
                            $bgcolor="#66FF66";
                    echo "<td bgcolor='".$bgcolor."'><center>\n";
                    echo "<a href='builds/".$arch_list[$i]."_medios_".$cur_date.".zip'>ZIP</a>\n";
                    
                    /*if($arch_list[$i]=="apps")
                    {
                            echo " - build <a href='builds/apps_build_".$cur_date.".log'>".$nb_build[$cur_date]["apps"];
                            echo "/".$nb_src[$cur_date]["apps"];
                            echo "</a> apps\n";
                    }*/
                    echo "<br>";
                    echo "<a href='builds/".$arch_list[$i]."_reportMedios_".$cur_date.".log'>\n";
                    echo "Err:".$error[$cur_date][$arch_list[$i]];
                    echo "-Warn:".$warning[$cur_date][$arch_list[$i]];
                    echo "</a><br>\n";
                    echo "<a href='builds/".$arch_list[$i]."_makeMedios_".$cur_date.".log'>Make log</a>\n";
                    echo "</center></td>\n";
                    
               }
                else
                {
                    echo "<td bgcolor='#6699FF'><center>No<br>Daily<br>build</center></td>\n";
                }
            }
	}
	else
	{
		echo "<td bgcolor='#6699FF' colspan='".$nb_arch."'><center>No Daily build available</center></td>\n";
	}
	echo "</tr>\n";
}
?>

</tr>
</tbody>
</table>
</center>
<p>
<DIV ALIGN=right><a href='http://www.archopen.org/tiki-index.php'>Back to index</a></div>
</body>
</html>