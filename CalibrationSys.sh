for iEn in "1GeV" "2GeV"
do
	for iMethod in "MeanCalibMethod" "EdgeCalibMethod"
	do
		root6 -b -q CalculateADCAmplitude.C\(\"${iEn}\",\"${iMethod}\"\)
	done
done