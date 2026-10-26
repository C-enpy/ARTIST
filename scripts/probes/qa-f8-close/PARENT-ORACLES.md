# Oracles figés avant logique parent/enfant

Décision75/76 : caractérisation bornée, GAP RED comportemental du superviseur
conservé. Aucun RED runtime fabriqué. Runner lead seulement après intégration.

- Usage invalide, argument dupliqué/inconnu, root relatif ou existant : exit20,
  aucun enfant et aucun changement de fixture/voisin préexistant.
- Parent acquiert root neuf et lease avant spawn. Nominal puis protégé, pas
  de bras suivant après setup, identité ou nominal invalide.
- Données : exactement4608 octets `A + i % 26`, lecture complète et EOF distinct
  de ferror avant protection ; une seule tentative fclose après fopen réussi.
- Le prédicat existant et les deux TU sont immuables. Les trois attentes
  (initiale, tardive, destruction) doivent appeler réellement ce prédicat avant
  collecte/cleanup. Timeout n'est pas terminaison. WAIT_FAILED conserve les
  handles et bail, marker puis Sleep(INFINITE), pas de kill ni cleanup.
- Phases/seq/run-id/PID/ctime et logs bornés ; fclose ENTER avant appel,
  retour/GLE/errno capturés avant logging, aucune utilisation FILE/fd/emprunt
  après tentative. Exception native : pas de handler/retry/consommation inventée.
- Exit0 seulement si nominal conforme, protection confirmée, EOF de fclose
  protégé observé, deux processus signalés et cleanup vérifié. Fclose0 protégé
  ou exception/inconclusive :10 ; setup/identité/nominal :20 ; instrumentation
  ou cleanup :30 ; complétion tardive :40. Jamais PASS Reader/syscall interne.
- Champs finaux indépendants : observation_complete, protected_fclose_return,
  refusal_confirmed_at_crt_boundary, cleanup_complete, reader_proven=false.
- Identité : adresse réellement appelée et module/base/chemin non tronqué,
  module EXE/thunk ambigu refusé. Imports/version/hash DLL à recouper par lead.
- Voisin conservé, suppression uniquement target puis fixture après signal ;
  logs/root/lease restent artefacts. Aucun test ne fabrique un handle invalide,
  exception ou deadlock. Ces branches restent revue statique + gap runtime.

Commandes exactes COMPILE à dériver des argv existants ; RUN exclusivement
functions.shell background=true timeout=0, jamais run-build.py timeout1200.
