# pool-17 (@correctness-reviewer teammate)

Session: ses_e74e7da65ffeUclVgr7J0qH30a
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-30T04:36:20.654Z — msg_18b18302a001C4sE1cnZvyTHMa

Je vais vérifier le contrat et le candidat épinglé, puis les régions WRAPPERS/NATIVE/DISPATCH et leurs fixtures. Revue en lecture seule, sans exécuter le helper ni les tests ; le raccordement commun futur reste hors périmètre.

## assistant — 2026-10-30T04:38:31.249Z — msg_18b1a2e4d001Cstnp9mQDW8UQo

Le worktree est propre à `78c91d6`. Le refus global précède toujours le dispatch : aucune phase de vérification ne peut atteindre le bootstrap par ce chemin. J’ai suivi les consommateurs factices ; je recoupe maintenant leurs preuves et les empreintes, sans relance. La capture privée et ses limites sur les handles hérités restent distinctes du futur run commun.

## assistant — 2026-10-30T04:38:51.357Z — msg_18b1a7cd9001C8Nxt4sQJwXFzs

## Avis : favorable à l’intégration intermédiaire

**Aucun nouveau défaut bloquant démontré dans le périmètre relu.** Cet avis n’ouvre pas le helper complet et ne qualifie pas le pipeline final.

### Ancrage et preuves
- Candidat : `78c91d698be97b7d38ac780971924e0c246ee5ac` ; worktree propre lors du contrôle.
- SHA256 de `scripts/build-windows.ps1` : `54c8d6d86c2644dd58a92a32d0c4fd60a147e03c2fbf6bfd83c475d546998968`.
- Région NATIVE : empreinte `8c9a56ffeb6f082e79bbe569ac519cbbf31cc3c2002e988aac9a82e61216e3de`, conforme au pin ; **le pin est une admission du source, pas une sandbox**.
- Les dix empreintes contrôlées correspondent au manifeste avant/après du lead.
- Journaux recoupés sous `out/verification-synthetic/windows-observation-v2-object-compare-2026-10-30T04-34-29-285Z/` : **1 + 13 + 37 tests PASS**, exécutés par le lead. Aucune relance par moi.

### Localisation / impact examinés
Dans `scripts/build-windows.ps1` :
- `Resolve-VerificationInputs`, ligne 17, et quoting, ligne 107 : champs fermés, limites Unicode et encodage des arguments.
- `Read-VerificationPayloadFile`, ligne 172 : lecture bornée à 1 MiB, UTF‑8 strict, doublons et échappements, containment et préflight des ancêtres reparse.
- `Resolve-VerificationPhaseFlags`, ligne 301 : phases explicites, incompatibilités et opt-in prepare.
- `New-VerificationNativeStartInfo`, ligne 361 : longueur complète UTF‑16, environnement enfant explicite, sans mutation du parent.
- `Invoke-VerificationNativeProcess`, ligne 405 : objet fourni conservé, arrêt du seul primaire, erreurs de capture explicites et journaux sans écrasement.
- Refus global, lignes 516–518 : toute `VerificationPhase` est arrêtée avant le dispatch et le bootstrap.

Consommateurs suivis : fixtures native, wrapper et dispatch, ainsi que leurs assertions pertinentes. Le snapshot parent sérialisé conserve la distinction vide/absent ; les getters diagnostiques ne constituent plus l’oracle.

### Limites conservées — non nouveaux findings
- `Invoke-VerificationNativeProcess`, lignes 489–498 : fragilité déjà connue du `Dispose` synchrone avant attente d’une lecture, notamment avec writers hérités. Aucun délai absolu ni comportement avec descendants démontré.
- Test06 jonction **NON_EXECUTE** ; préflight reparse non qualifié dynamiquement, sans contournement des skips.
- Capture/helper-run privés **SOURCE_ONLY**, hors critère du futur run commun direct.
- Raccordement prepare/build différé conformément à `windows-adapter-binding-v1.md` ; son absence n’est pas un défaut de cette tranche.
- Serena voit le dépôt canonique, pas ce worktree : conclusions fondées sur les fichiers du candidat, sans prétention de couverture LSP.

Aucune édition, exécution de helper/test/build/workload, ni activité moteur/GL/cloud. Aucun renvoi spécialiste nouveau nécessaire. **L’absence de finding n’est pas une preuve exhaustive.**