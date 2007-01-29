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

<H1>ArchOpen Daily Builds</h1>

<a href='/tiki-index.php'>back</a><p>
<table border="1">
  <tbody>
    <tr>
      <td>Date</td>
<?

$arch_list=array("jbmm","av1xx","av3xx","g4xx","g402","av4xx","pma","apps");
$arch_name=array("jbmm"=>"Jbmm","av1xx"=>"Av 1xx","av3xx"=>"Av 3xx","g4xx"=>"Gmini 4XX","g402"=>"Gmini 402","av4xx"=>"Av 4xx","pma"=>"PMA","av5xx"=>"Ax 5xx","apps"=>"Med Applications");
$nb_arch=count($arch_list);

for($i=0;$i<$nb_arch;$i++)
{
	echo "<td align='center'>".$arch_name[$arch_list[$i]]."</td>\n";
}
?>
</tr>
<?
for($j=0;$j<$nb_days;$j++)
{
	echo "<tr>\n";
	$cur_date=$str[$j];
	echo "<td>".$cur_date."</td>\n";
	if($has_file[$j])
	{
            for($i=0;$i<$nb_arch;$i++)
            {
                if(array_key_exists($arch_list[$i],$warning[$cur_date]))
                {
                    if($error[$cur_date][$arch_list[$i]]>0)
                            $bgcolor="red";
                    else if($warning[$cur_date][$arch_list[$i]]>0)
                            $bgcolor="orange";
                    else
                            $bgcolor="cyan";
                    echo "<td bgcolor='".$bgcolor."'>\n";
                   /* if($arch_list[$i]=="g402")
                    {
                            echo "<a href='".$arch_list[$i]."_medios_".$cur_date.".zip'>Bin</a>&nbsp;&nbsp;\n";
                            echo "<a href='cc_".$arch_list[$i]."_medios_".$cur_date.".zip'>cc Bin</a>\n";
                    }
                    else
                    {*/
                            echo "<a href='builds/".$arch_list[$i]."_medios_".$cur_date.".zip'>Bin</a>\n";
                    //}
                    
                    if($arch_list[$i]=="apps")
                    {
                            echo " - build <a href='builds/apps_build_".$cur_date.".log'>".$nb_build[$cur_date]["apps"];
                            echo "/".$nb_src[$cur_date]["apps"];
                            echo "</a> apps\n";
                    }
                    echo "<br>";
                    echo "<a href='builds/".$arch_list[$i]."_reportMedios_".$cur_date.".log'>E:".$error[$cur_date][$arch_list[$i]]."</a>\n";
                    echo "<a href='builds/".$arch_list[$i]."_reportMedios_".$cur_date.".log'>W:".$warning[$cur_date][$arch_list[$i]]."</a><br>\n";
                    echo "<a href='builds/".$arch_list[$i]."_makeMedios_".$cur_date.".log'>Make log</a>\n";
                    echo "</td>\n";
                    
               }
                else
                {
                    echo "<td>No Data</td>\n";
                }
            }
	}
	else
	{
		echo "<td colspan='".$nb_arch."'>No data</td>\n";
	}
	echo "</tr>\n";
}
?>

</tr>
</tbody>
</table>
<p><a href='/tiki-index.php'>back</a>
</body>
</html>