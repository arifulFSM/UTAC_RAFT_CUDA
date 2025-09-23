#include "pch.h"
#include "MTH/Define.h"
#include "PSpar.h"

void WLI::SPSpar::SetConst(float wlR, float wlG, float wlW, float UStep) {
	Set(wlR, wlG, wlW, UStep);
	WLE_um = wlR * wlG / fabsf(wlR - wlG);
	wnum = 1 / wlG;
	dwnum = 1 / wlG - 1 / wlR;
	sf1 = PIE4 * wnum;
	sf2 = 1 / (PIE4 * dwnum);
}

void WLI::SPSpar::Set(WLI::FRP Ch, float wl, float us) {
	UStep_um = us;
	WL1_um = wl;
	// desire
	PS[Ch] = wl / 4.f;
	Inc[Ch] = short(PS[Ch] / UStep_um + 0.5f); if (Inc[Ch] < 1) Inc[Ch] = 1;
	// calculated
	PS[Ch] = UStep_um * Inc[Ch]; PSrd[Ch] = PIE2 * (PS[Ch] / wl);
	PSsin[Ch] = float(2 * sin(PSrd[Ch]));
}

void WLI::SPSpar::Set(float wlr, float wlg, float wlw, float us) {
	UStep_um = us;
	WL1_um = wlr; WL2_um = wlg; WL3_um = wlw;
	// desire
	PS[FRP::REDA] = wlr / 4.f; PS[FRP::GRNA] = wlg / 4.f; PS[FRP::WHTA] = wlw / 4.f;

	Inc[int(FRP::REDA)] = short(PS[FRP::REDA] / UStep_um + 0.5f); if (Inc[int(FRP::REDA)] < 1) Inc[int(FRP::REDA)] = 1;
	Inc[int(FRP::GRNA)] = short(PS[FRP::GRNA] / UStep_um + 0.5f); if (Inc[int(FRP::GRNA)] < 1) Inc[int(FRP::GRNA)] = 1;
	Inc[int(FRP::WHTA)] = short(PS[FRP::WHTA] / UStep_um + 0.5f); if (Inc[int(FRP::WHTA)] < 1) Inc[int(FRP::WHTA)] = 1;

	// calculate
	PS[FRP::REDA] = UStep_um * Inc[int(FRP::REDA)]; 
	PSrd[int(FRP::REDA)] = PIE2 * (PS[FRP::REDA] / wlr);
	
	PS[FRP::GRNA] = UStep_um * Inc[int(FRP::GRNA)]; 
	PSrd[int(FRP::GRNA)] = PIE2 * (PS[FRP::GRNA] / wlg);
	
	PS[FRP::WHTA] = UStep_um * Inc[int(FRP::WHTA)]; 
	PSrd[int(FRP::WHTA)] = PIE2 * (PS[FRP::WHTA] / wlw);

	PSsin[FRP::REDA] = float(2 * sin(PSrd[int(FRP::REDA)]));
	PSsin[FRP::GRNA] = float(2 * sin(PSrd[int(FRP::GRNA)]));
	PSsin[FRP::WHTA] = float(2 * sin(PSrd[int(FRP::WHTA)]));
}

void WLI::SPSpar::dump(FILE* fp) {
	fprintf(fp, "Unit Step(um(pz)),%.4f\n", UStep_um);
	fprintf(fp, "Wavelengths(um(pz))\nwl1,%.4f,wl2,%.4f,wle,%.4f\n", WL1_um, WL2_um, WLE_um);
	fprintf(fp, "Phase shifts\npsR(um(pz)),%.4f,psR(rad),%.4f,psR(sin),%.4f\n", PS[FRP::REDA], PSrd[FRP::REDA], PSsin[FRP::REDA]);
	fprintf(fp, "psG(um(pz)),%.4f,psG(rad),%.4f,psG(sin),%.4f\n", PS[FRP::GRNA], PSrd[FRP::GRNA], PSsin[FRP::GRNA]);
	fprintf(fp, "psW(um(pz)),%.4f,psW(rad),%.4f,psW(sin),%.4f\n", PS[FRP::WHTA], PSrd[FRP::WHTA], PSsin[FRP::WHTA]);
	fprintf(fp, "wave number,%.4f,delta wave number,%.4f\n", wnum, dwnum);
	fprintf(fp, "incR,%d,incG,%d,incW,%d\n", Inc[int(FRP::REDA)], Inc[int(FRP::GRNA)], Inc[int(FRP::WHTA)]);
	fprintf(fp, "sf1,%.6f,sf2,%.6f\n", sf1, sf2);
}

float WLI::SPSpar::Get(WLI::SPSpar& PsP, WLI::FRP Ch) {
	if ((Ch < WLI::REDA) || (Ch > WLI::WHTA)) {
		ASSERT(0); return 0;
	}
	return PsP.PSsin[Ch];
}

CString WLI::SPSpar::Report() {
	CString str;
	str.Format(L"\nPhase shift (um): R:%.4f,  G:%.4f,  B:%.4f,  W:%.4f\nInc: R:%d,  G:%d,  B:%d,  W:%d\n",
		PS[FRP::REDA] * 2, PS[FRP::GRNA] * 2, PS[FRP::BLUA] * 2, PS[FRP::WHTA] * 2,
		Inc[FRP::REDA], Inc[FRP::GRNA], Inc[FRP::BLUA], Inc[FRP::WHTA]);
	return str;
}