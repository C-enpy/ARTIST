# GETTERS_FIX_READY — source, fixture, pin separes ; vide encore ouvert

## Preuve lead reutilisee

Tentative produit sur 90c26f8 : selfcheck14 PASS ; 13 methodes, 14 failures,
zero error/skip. Anciens37 NON reexecutes apres echec. Diagnostic lead suivant :
`out/verification-synthetic/windows-env-getters-diagnosis-2026-10-30T03-32-22-311Z/`.
Traces existantes relues par l'auteur via Context Mode, aucun nouveau run.

- .Id et .HasExited adaptes donnent null sur Process non associe ;
  .get_Id() et .get_HasExited() levent InvalidOperationException comme attendu.
- Fonction pure sans produit : vide present initialement, affectation directe
  le conserve, appel/retour de fonction le perd ; temoins keep/absent corrects.
- La cellule getters sans Start/Kill conserve le vide. Ne pas attribuer sa perte
  au parser/constructeur natif ; ne pas normaliser vide=absent ni restaurer l'env.

## Diffs exacts a revoir

1. Produit : `4361426bdb660e41ad9349afb0b754e0a5106f12`, trois substitutions
   uniquement : preflight get_Id, boucle et finally get_HasExited. L'association
   n'est reconnue qu'apres getter .NET reussi ; pas apres null d'adapter.
2. Fixture : `9554672bac101a07a58b06a12531b765fcbcc14d`, deux substitutions
   uniquement : cleanup selfcheck et capture get_HasExited. Un objet non associe
   leve, conserve owned_alive_after_call=false et suit le catch existant ; ce
   n'est pas une preuve de vie ni un motif pour tuer un PID suppose.
3. Diagnostic reduit : `416e44221115fcfa12da80294af8e88b01aa7fa9` et
   RETURN_BOUNDARY_PROPOSAL.md. Programme separe propose, pas du code produit
   charge via un pin modifie. Une cellule puis au plus un temoin en host frais,
   tous marqueurs INLINE ; pas la precedente campagne de cinq cellules.
4. Pin/admission : diff separe, source native SHA256
   `8c9a56ffeb6f082e79bbe569ac519cbbf31cc3c2002e988aac9a82e61216e3de`.
   La source complete ET la fixture sont a relire, pas seulement l'egalite du pin.

15 tests natifs/assertions, anciens37, autres fixtures/audits et enfant inchanges.
Aucun delai de cleanup/drain relache, quota/repli introduit ou assertion de vie
affaiblie. Aucun changement de flag/helper complet/core/Windows adapter/GL.

## Prochaine experience proposee, runner lead seulement

RETURN_BOUNDARY_PROPOSAL.md contient invocation et environnement assaini requis.
Mode=function d'abord : caller-before -> callee-entry -> callee-before-return
-> caller-after, snapshots inlines sans fonction de trace ni serialization JSON.
Si perte a l'entree, temoin dot-source ; si perte au retour, InvokeReturnAsIs ;
au plus UN des deux en processus frais. Aucun enfant, Process/Start/Kill ou
restauration d'environnement dans ce probe. Si instrumentation invalide la
reproduction, arreter et rendre cette limite au lead, pas multiplier les modes.

Un temoin positif pourrait justifier une proposition locale de semantique de
callsite avec nouveau contrat de preuve. Il ne constitue pas deja ce correctif,
une modification de host global ou l'autorisation de changer les tests.
Si aucun chemin admissible ne preserve le vide, presenter explicitement cette
limite au lead pour arbitrage sans elargir l'objectif ou masquer la regression.

## Matrice reelle / limites

- Auteur : analyse de traces EXISTANTES, syntaxe/AST et comparaison statiques.
- Produit/fixture getters corriges et probe : NON_EXECUTE par l'auteur.
- Nouvelle preuve fonctionnelle GREEN : aucune. La perte du vide reste a isoler
  entre entree et retour ; le correctif getter ne la pretend pas resolue.
- Les 37 anciens restent non rerun ; cas06 et nouvelles jonctions NON_EXECUTE.
- Dispose lecteur avant Wait reste synchrone/fragile face aux writers herites ;
  aucune reclamation generale ni delai absolu prouve. Revue independante requise
  avant capture de production/helper complet, qui demeure bloque.
- Aucun moteur/GL/MSVC/vcvars/outils/build/install/cloud ; aucun droit nouveau.
