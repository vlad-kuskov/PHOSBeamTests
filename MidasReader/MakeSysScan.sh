for iCalibEn in "-1GeV" "-2GeV"
do
	for iCalibMethod in "-MeanCalibMethod" "-EdgeCalibMethod"
	do
		for iGC in "-GC0" "-GC1" "-GC2"
		do
			bash MakeScan_51V.sh ${iCalibEn} ${iCalibMethod} ${iGC}
		done
	done
done