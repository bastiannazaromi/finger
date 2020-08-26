<?php
defined('BASEPATH') OR exit('No direct script access allowed');

class M_Data extends CI_Model {

	public function save($suhu)
	{
        $this->db->insert('tbrekap', $data);
    }

    public function ambil_data_terakhir()
    {
        $this->db->select('*');
        $this->db->from('tbuser');
        $this->db->order_by('id', 'desc');
        $this->db->limit(1);

        return $this->db->get()->result_array();
    }

    public function getAll()
    {
        return $this->db->get('tbuser')->result_array();
    }

    public function getAkses()
    {
        return $this->db->get('tbakses')->result_array();
    }

    public function hapus($id)
    {
        // $this->db->where('id', $id);
        $this->db->delete('tbuser', ['id' => $id]);
    }

    public function editAkses()
    {
        $akses = $this->input->post('akses');

        $data = [
            "akses" => $akses
        ];

        $this->db->update('tbakses', $data);
    }

    public function editUser()
    {
        $nama = $this->input->post('nama');

        $data = [
            "nama" => $nama
        ];

        $this->db->where('id', $this->input->post('id'));
        $this->db->update('tbuser', $data);
    }
}