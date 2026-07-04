// VampireSurvivors.Runtime, Version=0.0.0.0, Culture=neutral, PublicKeyToken=null
// VampireSurvivors.Objects.Weapons.MagicMissileWeapon
using System;
using VampireSurvivors.Data.Characters;
using VampireSurvivors.Objects.Weapons;

public class MagicMissileWeapon : Weapon
{
	public float ProjectileYOffset;

	protected override void MakeLevelOne()
	{
		//IL_001c: Expected O, but got I4
		Console.WriteLine("");
		((Weapon)0).MakeLevelOne();
		bool flag = new string("") == null;
		CharacterData characterData = (CharacterData)(object)new string("");
		if (!flag)
		{
			bool flag2 = (object)((CharacterData)(object)this).<zeroPad>k__BackingField == null;
			characterData = (CharacterData)((CharacterData)(object)this).<zeroPad>k__BackingField;
			if (flag2)
			{
			}
		}
		Console.WriteLine("Method not found @180AB7550");
	}

	public override void CheckArcanas()
	{
		/*Error: Empty body found. Decompiled assembly might be a reference assembly.*/;
	}

	public MagicMissileWeapon()
	{
		//IL_0041: Expected I, but got O
		//IL_008c: Expected O, but got I4
		//IL_0038->IL0038: Incompatible stack heights: 1 vs 0
		if ((long)new string("") == 0L)
		{
			Console.WriteLine("瀀渀最∀Ⰰ\u2000渀愀洀攀\u2000䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀");
			_ = 1L;
		}
		nint num = (nint)new Weapon();
		if ((long)new string("渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㌀㌀\u2000䀀\u2000爀愀砀开瘀㌀Ⰰ\u2000∀∀Ⰰ\u2000渀愀洀攀\u2000䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀洀⸀匀琀爀椀") == 0L)
		{
			Console.WriteLine("");
		}
		Console.WriteLine("");
		((Weapon)0)..ctor();
	}
}
