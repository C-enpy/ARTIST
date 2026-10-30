# 126 — Collecte privée : RED confirmé avant correction

Le pipeline doit conserver les preuves privées du helper avant de publier un reçu
de préparation ou de construction. Le coordinateur a terminé la relecture des
vingt tests et de leur fixture synthétique, puis vérifié le candidat propre :
seuls les deux ajouts annoncés sont présents, sans modification de production
ou des fakes historiques.

Les deux baselines passent : commandes génériques inchangées et refus sans
autorisation de préparation. Le test de collecte donne ensuite **un test et deux
échecs attendus**, Debug et Release, sans erreur ni skip. La cause est précise :
les fichiers privés fermés ne sont pas empreintés dans les artefacts avant
publication du reçu. Les baux de propriété et le faux helper ont bien été atteints.
Les assertions de construction et les autres négatifs encore masqués ne sont
pas présentés comme des RED individuellement constatés.

Le GREEN minimal du collecteur est autorisé sur les hooks privés du cœur et
les données des deux fakes déjà mappés. Les vingt tests et leurs assertions
restent inchangés. Les causes et captures communes doivent être préservées,
y compris lors d'une collecte partielle après erreur. Relecture, tests du lead
et revue indépendante précéderont l'intégration.

Le producteur PowerShell reste indépendant ; le helper est toujours fermé.
La couture finale attendra les deux producteurs effectivement intégrés.

## Preuves et portée

- Candidat `a3dfc1de39213ede20955d66ea052a14573d4295`, auteur pool-34 ; exécutant coordinateur.
- Cwd : `scripts/tests/verification_core` du worktree pool-34.
- Commandes : `C:/Python313/python.exe -B -m unittest -v` avec les deux méthodes C6 `test_C6_generic_commands_without_helper_transport_remain_unchanged` et `test_C6_opt_in_refusal_cannot_activate_helper_collection`, puis séparément la méthode C1 `test_C1_prepare_and_build_collect_closed_exact_bytes_under_lease_before_receipts`, toutes préfixées par `test_windows_helper_evidence_contract.WindowsHelperEvidenceContractTests.`.
- Baseline : `out/verification-synthetic/windows-helper-core-baseline-2026-10-30T09-02-55-425Z/`.
- RED : `out/verification-synthetic/windows-helper-core-red-2026-10-30T09-03-05-146Z/`.
- Empreintes avant/après identiques ; HEAD lead avant rapport `5538332`. Les changements préexistants de profils agents restent exclus.

Aucun processus enfant, PowerShell, outil natif, moteur, rendu, benchmark,
installation ou cloud lancé par ces tests. Windows/Linux natifs et GPU NON_EXECUTE.
