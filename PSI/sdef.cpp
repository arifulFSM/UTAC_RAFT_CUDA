#include "pch.h"
#include "sdef.h"

std::string WLI::SZp::FrpHdr[int(FRP::LASTF)] = {
	"Red", "Green", "Blue","White",
	"Phase1", "UPhase1", "Phase2", "UPhase2",
	"EPhase", "UEPhase", "Order",
	"Visib1","Visib2",
	"Result", "Tmp1","Tmp2","Tmp3",
	"ZAxis"
};

void WLI::SZp::clear() {
	zaxs.clear();
	reda.clear(); grna.clear(); blua.clear(); whta.clear();
	phs1.clear(); phs2.clear(); phse.clear();
	uph1.clear(); uph2.clear(); uphe.clear();
	ordr.clear(); vis1.clear(); vis2.clear(); rslt.clear();
	tmp1.clear(); tmp2.clear(); tmp3.clear();
}

void WLI::SZp::clear(WLI::FRP Ch) {
	switch (Ch) {
	case WLI::REDA: reda.clear(); break;
	case WLI::GRNA: grna.clear(); break;
	case WLI::BLUA: blua.clear(); break;
	case WLI::WHTA: whta.clear(); break;
	case WLI::PHS1: phs1.clear(); break;
	case WLI::UPH1: uph1.clear(); break;
	case WLI::PHS2: phs2.clear(); break;
	case WLI::UPH2: uph2.clear(); break;
	case WLI::PHSE: phse.clear(); break;
	case WLI::UPHE: uphe.clear(); break;
	case WLI::ORDR: ordr.clear(); break;
	case WLI::VIS1: vis1.clear(); break;
	case WLI::VIS2: vis2.clear(); break;
	case WLI::RSLT: rslt.clear(); break;
	case WLI::TMP1: tmp1.clear(); break;
	case WLI::TMP2: tmp2.clear(); break;
	case WLI::TMP3: tmp3.clear(); break;
	case WLI::ZAXS: zaxs.clear(); break;
	default: ASSERT(0); break;
	}
}

void WLI::SZp::resize(WLI::FRP Ch, int sz) {
	switch (Ch) {
	case WLI::ZAXS: zaxs.resize(sz); break;
	case WLI::REDA: reda.resize(sz); break;
	case WLI::GRNA: grna.resize(sz); break;
	case WLI::BLUA: blua.resize(sz); break;
	case WLI::WHTA: whta.resize(sz); break;
	case WLI::PHS1: phs1.resize(sz); break;
	case WLI::UPH1: uph1.resize(sz); break;
	case WLI::PHS2: phs2.resize(sz); break;
	case WLI::UPH2: uph2.resize(sz); break;
	case WLI::PHSE: phse.resize(sz); break;
	case WLI::UPHE: uphe.resize(sz); break;
	case WLI::ORDR: ordr.resize(sz); break;
	case WLI::VIS1: vis1.resize(sz); break;
	case WLI::VIS2: vis2.resize(sz); break;
	case WLI::RSLT: rslt.resize(sz); break;
	case WLI::TMP1: tmp1.resize(sz); break;
	case WLI::TMP2: tmp2.resize(sz); break;
	case WLI::TMP3: tmp3.resize(sz); break;
	default: ASSERT(0); break;
	}
}

bool WLI::SZp::IsAllc(WLI::FRP Ch) {
	switch (Ch) {
	case WLI::REDA: return reda.size() > 0; break;
	case WLI::GRNA: return grna.size() > 0; break;
	case WLI::BLUA: return blua.size() > 0; break;
	case WLI::WHTA: return whta.size() > 0; break;
	case WLI::PHS1: return phs1.size() > 0; break;
	case WLI::UPH1: return uph1.size() > 0; break;
	case WLI::PHS2: return phs2.size() > 0; break;
	case WLI::UPH2: return uph2.size() > 0; break;
	case WLI::PHSE: return phse.size() > 0; break;
	case WLI::UPHE: return uphe.size() > 0; break;
	case WLI::ORDR: return ordr.size() > 0; break;
	case WLI::VIS1: return vis1.size() > 0; break;
	case WLI::VIS2: return vis2.size() > 0; break;
	case WLI::RSLT: return rslt.size() > 0; break;
	case WLI::TMP1: return tmp1.size() > 0; break;
	case WLI::TMP2: return tmp2.size() > 0; break;
	case WLI::TMP3: return tmp3.size() > 0; break;
	case WLI::ZAXS: return zaxs.size() > 0; break;
	default: ASSERT(0); break;
	}
	return false;
}

int WLI::SZp::size() {
	return int(zaxs.size());
}

int WLI::SZp::size(WLI::FRP Ch) {
	switch (Ch) {
	case WLI::REDA: return int(reda.size()); break;
	case WLI::GRNA: return int(grna.size()); break;
	case WLI::BLUA: return int(blua.size()); break;
	case WLI::WHTA: return int(whta.size()); break;
	case WLI::PHS1: return int(phs1.size()); break;
	case WLI::UPH1: return int(uph1.size()); break;
	case WLI::PHS2: return int(phs2.size()); break;
	case WLI::UPH2: return int(uph2.size()); break;
	case WLI::PHSE: return int(phse.size()); break;
	case WLI::UPHE: return int(uphe.size()); break;
	case WLI::ORDR: return int(ordr.size()); break;
	case WLI::VIS1: return int(vis1.size()); break;
	case WLI::VIS2: return int(vis2.size()); break;
	case WLI::RSLT: return int(rslt.size()); break;
	case WLI::TMP1: return int(tmp1.size()); break;
	case WLI::TMP2: return int(tmp2.size()); break;
	case WLI::TMP3: return int(tmp3.size()); break;
	default: ASSERT(0); break;
	}
	return 0;
}

std::vector<float>& WLI::SZp::Get(WLI::FRP ch) {
	switch (ch) {
	case WLI::REDA: return reda; break;
	case WLI::GRNA: return grna; break;
	case WLI::BLUA: return blua; break;
	case WLI::WHTA: return whta; break;
	case WLI::PHS1: return phs1; break;
	case WLI::UPH1: return uph1; break;
	case WLI::PHS2: return phs2; break;
	case WLI::UPH2: return uph2; break;
	case WLI::PHSE: return phse; break;
	case WLI::UPHE: return uphe; break;
	case WLI::ORDR: return ordr; break;
	case WLI::VIS1: return vis1; break;
	case WLI::VIS2: return vis2; break;
	case WLI::RSLT: return rslt; break;
	case WLI::TMP1: return tmp1; break;
	case WLI::TMP2: return tmp2; break;
	case WLI::TMP3: return tmp3; break;
	}
	return zaxs;
}

// resize the vector to store data and return the vector pointer
float* WLI::SZp::Get(WLI::FRP Ch, int ist, int sz) {
	if (sz < 1) { ASSERT(0); return nullptr; }
	switch (Ch) {
	case WLI::ZAXS: if (Sz[Ch] != sz) { Sz[Ch] = sz; zaxs.resize(sz); } return&zaxs[ist]; break;
	case WLI::REDA: if (Sz[Ch] != sz) { Sz[Ch] = sz; reda.resize(sz); } return&reda[ist]; break;
	case WLI::GRNA: if (Sz[Ch] != sz) { Sz[Ch] = sz; grna.resize(sz); } return&grna[ist]; break;
	case WLI::BLUA: if (Sz[Ch] != sz) { Sz[Ch] = sz; blua.resize(sz); } return&blua[ist]; break;
	case WLI::WHTA: if (Sz[Ch] != sz) { Sz[Ch] = sz; whta.resize(sz); } return&whta[ist]; break;
	case WLI::PHS1: if (Sz[Ch] != sz) { Sz[Ch] = sz; phs1.resize(sz); } return&phs1[ist]; break;
	case WLI::UPH1: if (Sz[Ch] != sz) { Sz[Ch] = sz; uph1.resize(sz); } return&uph1[ist]; break;
	case WLI::PHS2: if (Sz[Ch] != sz) { Sz[Ch] = sz; phs2.resize(sz); } return&phs2[ist]; break;
	case WLI::UPH2: if (Sz[Ch] != sz) { Sz[Ch] = sz; uph2.resize(sz); } return&uph2[ist]; break;
	case WLI::PHSE: if (Sz[Ch] != sz) { Sz[Ch] = sz; phse.resize(sz); } return&phse[ist]; break;
	case WLI::UPHE: if (Sz[Ch] != sz) { Sz[Ch] = sz; uphe.resize(sz); } return&uphe[ist]; break;
	case WLI::ORDR: if (Sz[Ch] != sz) { Sz[Ch] = sz; ordr.resize(sz); } return&ordr[ist];  break;
	case WLI::VIS1: if (Sz[Ch] != sz) { Sz[Ch] = sz; vis1.resize(sz); } return&vis1[ist]; break;
	case WLI::VIS2: if (Sz[Ch] != sz) { Sz[Ch] = sz; vis2.resize(sz); } return&vis2[ist]; break;
	case WLI::RSLT: if (Sz[Ch] != sz) { Sz[Ch] = sz; rslt.resize(sz); } return&rslt[ist]; break;
	case WLI::TMP1: if (Sz[Ch] != sz) { Sz[Ch] = sz; tmp1.resize(sz); } return&tmp1[ist];  break;
	case WLI::TMP2: if (Sz[Ch] != sz) { Sz[Ch] = sz; tmp2.resize(sz); } return&tmp2[ist];  break;
	case WLI::TMP3: if (Sz[Ch] != sz) { Sz[Ch] = sz; tmp3.resize(sz); } return&tmp3[ist];  break;
	default: ASSERT(0); break;
	}
	return nullptr;
}

float* WLI::SZp::Get(WLI::FRP Ch, int ist) {
	// no sanity check
	switch (Ch) {
	case WLI::ZAXS: return&zaxs[ist]; break;
	case WLI::REDA: return&reda[ist]; break;
	case WLI::GRNA: return&grna[ist]; break;
	case WLI::BLUA: return&blua[ist]; break;
	case WLI::WHTA: return&whta[ist]; break;
	case WLI::PHS1: return&phs1[ist]; break;
	case WLI::UPH1: return&uph1[ist]; break;
	case WLI::PHS2: return&phs2[ist]; break;
	case WLI::UPH2: return&uph2[ist]; break;
	case WLI::PHSE: return&phse[ist]; break;
	case WLI::UPHE: return&uphe[ist]; break;
	case WLI::ORDR: return&ordr[ist];  break;
	case WLI::VIS1: return&vis1[ist]; break;
	case WLI::VIS2: return&vis2[ist]; break;
	case WLI::RSLT: return&rslt[ist]; break;
	case WLI::TMP1: return&tmp1[ist];  break;
	case WLI::TMP2: return&tmp2[ist];  break;
	case WLI::TMP3: return&tmp3[ist];  break;
	default: ASSERT(0); break;
	}
	return nullptr;
}