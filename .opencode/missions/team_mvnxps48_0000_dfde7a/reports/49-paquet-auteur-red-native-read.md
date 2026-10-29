# 49 — Paquet autonome auteur RED, sans archive externe requise

2026-10-26. Protocole44 adopté46, interface/états48. Producteur RO pool-5 arrêté nativement ; team_merge confirme **aucun commit ni changement non commité**, branche préservée. Aucun travail source à réintégrer. Le consommateur S_CODE reste à admettre fresh/isolation réelle avant GO.

## Snapshot hérité à reprendre sans correction pour RED

Source d'origine canonique `out/qa-f8/calibration-20261024-01/f8-lock-probe.cpp`,8935octets, SHA2569a067758f87c105aa0bb1c724a7f5faaa54f77db30d018839e13bcf8abf0ecae calculé lead. Extraits14–19/70–82 ci-dessous fournis pour éviter d'imposer accès externe/archives ignorées dans le worktree. Pas de lecture/recherche de l'archive nécessaire à l'auteur si indisponible ; aucune modification de l'archive.

```cpp
constexpr DWORD share = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
void need(bool ok, const char* why) { if (!ok) throw std::runtime_error(why); }
bool os(const char* op, BOOL ok) {
    DWORD e = ok ? ERROR_SUCCESS : GetLastError();
    std::cout << op << " ok=" << !!ok << " error=" << e << '\n'; return !!ok;
}
struct Native { bool ok; DWORD error, n; std::string bytes; };
Native nativeRead(const std::string& path) {
    HANDLE h = CreateFileA(path.c_str(), GENERIC_READ, share, nullptr, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    need(os("CreateFileA control", h != INVALID_HANDLE_VALUE), "control open failed");
    std::array<char, 8192> b{}; DWORD n = 0;
    BOOL ok = ReadFile(h, b.data(), static_cast<DWORD>(b.size()), &n, nullptr);
    DWORD e = ok ? ERROR_SUCCESS : GetLastError();
    std::cout << "ReadFile ok=" << !!ok << " error=" << e << " n=" << n << '\n';
    need(os("CloseHandle control", CloseHandle(h)), "control close failed");
    need(n <= b.size(), "invalid ReadFile count");
    return { !!ok, e, n, std::string(b.data(), n) };
}
```

Compiler avec NOMINMAX défini avant windows.h, headers standards nécessaires explicites (array,string,stdexcept,iostream...). Déplacer dans namespace/header local est mécanique ; **ne pas changer algorithme,8192,ReadFile unique/retour** avant preuve RED. Fonctions de fixture/contrôle neuves distinctes, pas d'altération de l'attendu12325.

## Livrable initial autorisable après admission vérifiée

Seulement `scripts/probes/qa-f8-stdio/controls.cpp` + helper local héritant exactement cet algorithme et note de protocole au même sous-arbre. Aucun probe.cpp, boucle corrigée, nouvelle seam ou test moteur à cette phase. Même propriétaire poursuivra GREEN après preuve lead.

CLI48 controls : --case native-read-whole --root ABS_NOUVEAU. Acquérir racine refusant l'existant, fichiers possédés0/4096/8192/12325 écrits exactement via Win32, attendus déterministes complets, aucun sizecheck/stat pour contourner la lecture. Quatre contrôles dans ce seul processus, tailles et octets ; signaler ReadFileBOOL/error/n et comparaison exacte. Les trois petits doivent passer ; cas12325 échoue pour8192 reçus, exit1. Acquisition/écriture/cleanup cassés sortent20/30, pas RED causal. Logs hors fixtures, voisins préservés. Ne pas introduire volontairement autre défaut de comparateur.

Racine/source/header disponibles au HEAD auteur ; compile_commands/wrapper canonique seront digérés par lead, aucun appel compilateur ni shell de build auteur. Sourcetests livrée/committée par auteur puis intégrée lead **avant** runnerRED ; aucune affirmation RED basée seulement sur lecture de code ou zéro test. Arrêt de phase et remise fragments<3500octetsUTF8. Pas taskcomplete global sonde/implémentation avant intégration/preuves, pas prochaine tâche auto-claim.

## Préconditions d'admission à constater, non présumées

Profil cpp-engineer, nouveau contexte natif sans historique auteur, worktree enregistré propre né après commits QA/38/44/46/48/49. Vérifier GETlocation.directory et Git réel ; si freshsession restée au canonique malgré worktree, native session_move vers **son propre** worktree puis GET/Git/lecture/restitution avant GO. Si impossible : BLOCKED, pas d'édition canonique ou de permissions élargies. Le texte fresh/worktree ne prouve pas leur application.

PM37 S_CODE brut68→70/R66, même routage approuvé, aucun modèle explicite ; source82bf52d inchangée. Indépendance reviewer/audit futur inchangée ; aucun flag de protectionclose ou nouveau périmètreOS dans ces contrôles.
